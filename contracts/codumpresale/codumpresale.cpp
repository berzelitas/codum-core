#include "codumpresale.hpp"

#include "../codum.token/codum.token.hpp"

int64_t codumpresale::get_bonus_state(const uint8_t stage) const
{
  asset sum = bonus_thr[stage];

  contributions contribution_table(_self, _self);
  for (auto ct = contribution_table.begin(); ct != contribution_table.end(); ct++)
  {
    if (ct->validated > 0)
    {
      sum -= ct->codum_dist;
    }
  }

  return sum.amount;
}

int64_t codumpresale::get_sale_state(const asset &cap) const
{
  asset sum = cap;

  contributions contribution_table(_self, _self);

  if (cap == softcap)
  {
    for (auto ct = contribution_table.begin(); ct != contribution_table.end(); ct++)
    {
      if (ct->validated > 0)
      {
        sum -= ct->codum_dist;
      }
    }
  }
  else
  {
    for (auto ct = contribution_table.begin(); ct != contribution_table.end(); ct++)
    {
      if (ct->distributed > 0)
      {
        sum -= ct->codum_dist;
      }
    }
  }

  return sum.amount;
}

bool codumpresale::is_contributor_approved(const account_name contributor) const
{
  whitelists whitelist_table(_self, _self);
  auto itr = whitelist_table.find(contributor);
  if (itr != whitelist_table.end() && itr->acceptedterms && itr->approved > 0)
  {
    return true;
  }
  return false;
}

void codumpresale::apply(const account_name contributor,
                         const bool acceptedterms)
{
  require_auth(contributor);

  whitelists whitelist_table(_self, _self);
  auto itr = whitelist_table.find(contributor);
  if (itr == whitelist_table.end())
  {
    // create
    whitelist_table.emplace(_self, [&](auto &wt) {
      wt.contributor = contributor;
      wt.acceptedterms = acceptedterms;
      wt.approved = 0;
    });
  }
  else
  {
    // update
    whitelist_table.modify(itr, _self, [&](auto &wt) {
      wt.contributor = contributor;
      wt.acceptedterms = acceptedterms;
    });
  }
}

void codumpresale::approve(const account_name contributor)
{
  require_auth(_self);
  eosio_assert(is_account(contributor), "contributor account does not exist");

  whitelists whitelist_table(_self, _self);
  auto itr = whitelist_table.find(contributor);

  eosio_assert(itr != whitelist_table.end(), "there is no such contributor in whitelist");
  eosio_assert(itr->acceptedterms, "contributor has not accepted terms");

  whitelist_table.modify(itr, _self, [&](auto &wt) {
    wt.approved = now();
  });
}

void codumpresale::buycodum(const account_name contributor,
                            const uint8_t network,
                            const asset &quantity,
                            const string &memo)
{
  require_auth(contributor);
  eosio_assert(get_sale_state(hardcap) > 1, "hard cap reached");
  eosio_assert(is_contributor_approved(contributor), "please wait while we approve your participation");
  eosio_assert(end > now(), "fundraising round A has ended");

  contributions contribution_table(_self, _self);

  auto contributorIndex = contribution_table.get_index<N(contributor)>();

  uint8_t nonvalContrsNumber = 0;
  auto lower = contributorIndex.lower_bound(contributor);
  auto upper = contributorIndex.upper_bound(contributor);
  
  for (auto itr = lower; itr != upper; itr++)
  {
    if (itr->validated == 0)
    {
      nonvalContrsNumber++;
    }
  }

  eosio_assert(nonvalContrsNumber < 3, "contributor cannot have more than 3 unverified transactions");

  contribution_table.emplace(_self, [&](auto &ct) {
    ct.id = contribution_table.available_primary_key();
    ct.contributor = contributor;
    ct.network = network;
    ct.quantity = quantity;
    ct.datetime = now();
    ct.memo = memo;
    ct.distributed = 0;
    ct.refunded = 0;

    token::exrates exrate_table(tokencontract, tokencontract);
    auto rt = exrate_table.find(network);
    eosio_assert(rt != exrate_table.end(), "no such network in codum.token exrates table");

    ct.rate = rt->rate;
    ct.codum_dist = asset(ct.quantity.amount * ct.rate / 10000, S(4, CODUM));
    ct.codum_bonus = asset(0, S(4, CODUM));
  });
}

void codumpresale::validate(const uint64_t id, const string &memo, const string &transaction)
{
  require_auth(_self);

  contributions contribution_table(_self, _self);
  auto itr = contribution_table.find(id);
  eosio_assert(itr != contribution_table.end(), "there is no contribution with this id");

  contribution_table.modify(itr, _self, [&](auto &ct) {
    eosio_assert(is_contributor_approved(ct.contributor), "please wait while we approve your participation");
    eosio_assert(memo == ct.memo, "transaction memo mismatch");
    eosio_assert(start < ct.datetime && end > ct.datetime, "date of the transaction outside sale date and time bounds");

    token::exrates exrate_table(tokencontract, tokencontract);
    auto rt = exrate_table.find(ct.network);
    eosio_assert(rt != exrate_table.end(), "no such network in codum.token exrates table");

    ct.rate = rt->rate;
    ct.codum_dist = asset(ct.quantity.amount * ct.rate / 10000, S(4, CODUM));
    ct.validated = now();
    ct.transaction = transaction;
    ct.codum_bonus.amount = 0;

    auto dist = ct.codum_dist.amount;
    int64_t last_bonus = 0;
    const uint8_t stages_count = 2;

    for (uint8_t stage = 0; dist > 0 && stage < stages_count; stage++)
    {
      auto left = get_bonus_state(stage) - last_bonus;
      if (dist <= left)
      {
        ct.codum_bonus.amount += dist * bonus[stage] / 100;
        break;
      }
      else if (left > 0)
      {
        ct.codum_bonus.amount += left * bonus[stage] / 100;
        dist -= left;
        last_bonus += left;
      }
    }
  });
}

void codumpresale::deletetx(const uint64_t id)
{
  require_auth(_self);
  contributions contribution_table(_self, _self);
  auto itr = contribution_table.find(id);
  eosio_assert(itr != contribution_table.end(), "there is no contribution with this id");
  eosio_assert(itr->validated > 0, "cannot delete validated contribution");
  contribution_table.erase(itr);
  eosio_assert(itr != contribution_table.end(), "contribution not deleted properly");
}

void codumpresale::distribute(const uint64_t id)
{
  eosio_assert(get_sale_state(softcap) < 0, "cannot distribute tokens before reaching soft cap");

  contributions contribution_table(_self, _self);
  auto itr = contribution_table.find(id);
  eosio_assert(itr != contribution_table.end(), "there is no contribution with this id");

  contribution_table.modify(itr, _self, [&](auto &dt) {
    eosio_assert(dt.validated > 0, "contribution is not validated");
    eosio_assert(dt.distributed == 0, "contribution is already distributed");
    eosio_assert(dt.refunded == 0, "contribution is refunded");

    auto hardcapLeft = get_sale_state(hardcap);

    if (hardcapLeft > 1)
    {
      auto dc = asset(0, S(4, CODUM));

      if (hardcapLeft < dt.codum_dist.amount)
      {
        dc.amount = hardcapLeft;
        dt.refund = asset(
          (dt.codum_dist.amount - dc.amount) * 10000 / dt.rate,
          dt.quantity.symbol
        );
      }
      else
      {
        dc = dt.codum_dist + dt.codum_bonus;
      }

      action(
          {_self, N(active)},
          tokencontract, N(distribsale),
          std::make_tuple(_self, dt.contributor, dc, dt.memo))
          .send();

      dt.distributed = now();
    }
    else
    {
      dt.refund = asset(dt.codum_dist.amount * 10000 / dt.rate, dt.quantity.symbol);
      dt.distrib_tx = "";
    }
  });
}

void codumpresale::refundsale(uint64_t id, string refund_tx)
{
  require_auth(_self);
  eosio_assert(get_sale_state(softcap) < 0 && (get_sale_state(hardcap) > 1), "cannot refund if soft cap reached but hard cap not");
  contributions contribution_table(_self, _self);

  auto r = contribution_table.find(id);
  eosio_assert(r != contribution_table.end(), "there is no contribution with this id");
  eosio_assert(r->refunded == 0, "contribution is already refunded");
  if (r->validated > 0 && r->refund.amount > 0)
  {
    if (r->network == NETWORK_EOS)
    {
      action(
          {N(codumpresale), N(active)},
          tokencontract, N(transfer),
          std::make_tuple(N(codumpresale), r->contributor, r->refund, r->memo))
          .send();
    }
    if (refund_tx != "")
    {
      contribution_table.modify(r, _self, [&](auto &val) {
        val.refunded = now();
        val.refund_tx = refund_tx;
      });
    }
  }
}


void codumpresale::finish()
{
  require_auth(_self);
  eosio_assert(end < now() || get_sale_state(hardcap) < 1, "presale is not ended yet");

  contributions contribution_table(_self, _self);

  if (get_sale_state(softcap) > 0)
  {
    for (auto itr = contribution_table.begin(); itr != contribution_table.end(); itr++)
    {
      if (itr->validated > 0)
      {
        contribution_table.modify(itr, _self, [&](auto &dt) {
          dt.refund = asset(dt.codum_dist.amount * 10000 / dt.rate, dt.quantity.symbol);
        });
      }
    }
  }
  else
  {
    for (const auto &ct: contribution_table)
    {
      if (ct.validated > 0)
      {
        eosio_assert(ct.distributed > 0 || ct.refunded > 0, "there is pending validated contribution");
      }
    }
  }
}

EOSIO_ABI(codumpresale, (apply)(approve)(buycodum)(validate)(deletetx)(distribute)(refundsale)(finish))
