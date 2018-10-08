#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

#include "mbps.hpp"

namespace eosio {

void bond::create(account_name issuer, asset maximum_supply) {
    require_auth(_self);

    auto sym = maximum_supply.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(maximum_supply.is_valid(), "invalid supply");
    eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable(_self, sym.name());
    auto existing = statstable.find(sym.name());
    eosio_assert(existing == statstable.end(), "token with symbol already exists");

    statstable.emplace(_self, [&](auto &s) {
        s.supply.symbol = maximum_supply.symbol;
        s.max_supply = maximum_supply;
        s.issuer = issuer;
        s.project = "";
    });
}

void bond::issue(account_name to, asset quantity, string memo) {
    auto sym = quantity.symbol;
    eosio_assert(sym.is_valid(), "invalid symbol name");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    auto sym_name = sym.name();
    stats statstable(_self, sym_name);
    auto existing = statstable.find(sym_name);
    eosio_assert(existing != statstable.end(), "token with symbol does not exist, create token before issue");
    const auto &st = *existing;

    require_auth(st.issuer);
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must issue positive quantity");

    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio_assert(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

    statstable.modify(st, 0, [&](auto &s) {
        s.supply += quantity;
    });

    add_balance(st.issuer, quantity, st.issuer);

    if (to != st.issuer) {
        SEND_INLINE_ACTION(*this, transfer, {st.issuer, N(active)}, {st.issuer, to, quantity, memo});
    }
}

void bond::distrtokens(id_type token_id) {
    // token bondtoken = tokens.find(token_id);
    // stat bs =
}

void bond::transfer(account_name from, account_name to, asset quantity, string memo) {
    eosio_assert(from != to, "cannot transfer to self");
    require_auth(from);
    eosio_assert(is_account(to), "to account does not exist");
    auto sym = quantity.symbol.name();
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);

    require_recipient(from);
    require_recipient(to);

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must transfer positive quantity");
    eosio_assert(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    sub_balance(from, quantity);
    add_balance(to, quantity, from);
}

void bond::transferid(account_name from, account_name to, id_type id, string memo) {
    // Ensure authorized to send from account
    eosio_assert(from != to, "cannot transfer to self");
    require_auth(from);

    // Ensure 'to' account exists
    eosio_assert(is_account(to), "to account does not exist");

    // Check memo size and print
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    // Ensure token ID exists
    auto sender_token = tokens.find(id);
    eosio_assert(sender_token != tokens.end(), "token with specified ID does not exist");

    // Ensure owner owns token
    eosio_assert(sender_token->owner == from, "sender does not own token with specified ID");

    const auto &st = *sender_token;

    // Notify both recipients
    require_recipient(from);
    require_recipient(to);

    // Transfer NFT from sender to receiver
    tokens.modify(st, from, [&](auto &token) {
        token.owner = to;
    });

    // Change balance of both accounts
    sub_balance(from, st.value);
    add_balance(to, st.value, from);
}

void bond::burn(account_name owner, id_type token_id) {
    require_auth(owner);

    // Find token to burn
    auto burn_token = tokens.find(token_id);
    eosio_assert(burn_token != tokens.end(), "token with id does not exist");
    eosio_assert(burn_token->owner == owner, "token not owned by account");

    asset burnt_supply = burn_token->value;

    // Remove token from tokens table
    tokens.erase(burn_token);

    // Lower balance from owner
    sub_balance(owner, burnt_supply);

    // Lower supply from currency
    sub_supply(burnt_supply);
}

void bond::setrampayer(account_name payer, id_type id) {
    require_auth(payer);

    // Ensure token ID exists
    auto payer_token = tokens.find(id);
    eosio_assert(payer_token != tokens.end(), "token with specified ID does not exist");

    // Ensure payer owns token
    eosio_assert(payer_token->owner == payer, "payer does not own token with specified ID");

    const auto &st = *payer_token;

    // Notify payer
    require_recipient(payer);

    // Set owner as a RAM payer
    tokens.modify(payer_token, payer, [&](auto &token) {
        token.id = st.id;
        token.uri = st.uri;
        token.owner = st.owner;
        token.value = st.value;
        token.name = st.name;
    });

    sub_balance(payer, st.value);
    add_balance(payer, st.value, payer);
}

void bond::mint(account_name owner, account_name ram_payer, asset value, string uri, string name) {
    // Add token with creator paying for RAM
    tokens.emplace(ram_payer, [&](auto &token) {
        token.id = tokens.available_primary_key();
        token.uri = uri;
        token.owner = owner;
        token.value = value;
        token.name = name;
    });
}

void bond::addmilestone(
  asset bond,
  uint16_t weight,
  time deadline,
  asset budget,
  asset token_budget,
  string milestone,
  string press_release_url,
  string epic_url
) {
    auto bond_symbol = bond.symbol;
    eosio_assert(bond_symbol.is_valid(), "Invalid symbol name.");
    eosio_assert(budget.symbol.is_valid(), "Invalid symbol name.");
    eosio_assert(token_budget.symbol.is_valid(), "Invalid symbol name.");

    stats statstable(_self, bond_symbol.name());
    auto bs = statstable.find(bond_symbol.name());

    eosio_assert(bs != statstable.end(), "Non-existed stats for bond.");
    require_auth(bs.token_contract);

    eosio_assert(deadline > bs.start && deadline < bs.end, "The deadline must be between of start and end.");

    eosio_assert(strlen(milestone) >= 16, "The milestone length must be at least 16 characters.");

    eosio_assert(bond.quantity == 0, "Bond quantity must be equal to 0.");

    eosio_assert(weight <= 1000000, "Total weight of all bond.symbol related milestones must not exceed 1000000.");

    accounts from_budget_acnts(bs.budget_contract, _self);
    const auto &budget_acc = from_budget_acnts.get(budget.symbol.name());
    eosio_assert(budget_acc.balance >= 0, "Budget must be a valid asset symbol with 0 or positive quantity.");

    accounts from_token_acnts(bs.token_contract, _self);
    const auto &token_acc = from_token_acnts.get(token_budget.symbol.name());
    eosio_assert(token_acc.balance >= 0, "Token budget must be a valid asset symbol with 0 or positive quantity.");

    schedules schedulable(_self, bond.symbol.name());
    schedulable.emplace(_self, [&](auto &s) {
        s.id = schedulable.available_primary_key();
        s.bond = bond;
        s.weight = weight;
        s.deadline = deadline;
        s.budget = budget;
        s.token_budget = token_budget;
        s.funding_status = 0;
        s.execution_status = 0;
    });
}

void bond::updatemilest(
  uint64_t id,
  uint16_t weight,
  time deadline,
  uint64_t budget,
  uint64_t token_budget,
  uint8 execution_status,
  string update_message,
  string press_release_url
) {
    schedules schedulable(_self, budget);
    const auto &m = schedulable.get(id, "No schedule object found.");

    stats statstable(_self, budget);
    auto bs = statstable.find(m.bond.symbol_name());

    eosio_assert(bs != statstable.end(), "Non-existed stats for bond.");

    require_auth(bs.token_contract);

    eosio_assert(m.execution_status < 0 || m.execution_status >= 100 || execution_status >= 0 && execution_status <= 100, "Wrong execution status.");

    eosio_assert(m.execution_status >= 100 && m.execution_status == execution_status, "Cannot update milestone execution status which is submitted for completion.");

    bool last_deadline = m.funding_status == 0;
    if (last_deadline) {
        auto itr = schedulable.lower_bound(statstable.start());
        for (; itr != accidx.end(); ++itr) {
            if (itr->id != id && itr->execution_status < 100 && itr->deadline > m.deadline) {
                last_deadline = false;
                break;
            }
        }
    }

    if (m.executation_status == 100) {
        eosio_assert(m.budget == budget, "Argument 'budget' is forbidden.");
        eosio_assert(m.deadline == deadline, "Argument 'deadline' is forbidden.");
        eosio_assert(m.token_budget == token_budget, "Argument 'token_budget' is forbidden.");
        eosio_assert(m.weight == weight, "Argument 'weight' is forbidden.");
    }

    schedulable.modify(m, _self, [&](auto &mt) {
        ms.deadline = deadline;

        if (last_deadline) {
            mt.weight = weight;
            mt.budget.amount = budget;
            mt.token_budget.amount = token_budget;
        }

        if (executation_status != 100) {
            mt.executation_status = executation_status;
        }
    });
}

void bond::sub_balance(account_name owner, asset value) {
    account_index from_acnts(_self, owner);
    const auto &from = from_acnts.get(value.symbol.name(), "no balance object found");
    eosio_assert(from.balance.amount >= value.amount, "overdrawn balance");

    if (from.balance.amount == value.amount) {
        from_acnts.erase(from);
    } else {
        from_acnts.modify(from, owner, [&](auto &a) {
            a.balance -= value;
        });
    }
}

void bond::add_balance(account_name owner, asset value, account_name ram_payer) {
    account_index to_acnts(_self, owner);
    auto to = to_acnts.find(value.symbol.name());
    if (to == to_acnts.end()) {
        to_acnts.emplace(ram_payer, [&](auto &a) {
            a.balance = value;
        });
    } else {
        to_acnts.modify(to, 0, [&](auto &a) {
            a.balance += value;
        });
    }
}

void bond::sub_supply(asset quantity) {
    auto symbol_name = quantity.symbol.name();
    stats currency_table(_self, symbol_name);
    auto current_currency = currency_table.find(symbol_name);

    currency_table.modify(current_currency, 0, [&](auto &currency) {
        currency.supply -= quantity;
    });
}

void bond::add_supply(asset quantity) {
    auto symbol_name = quantity.symbol.name();
    stats currency_table(_self, symbol_name);
    auto current_currency = currency_table.find(symbol_name);

    currency_table.modify(current_currency, 0, [&](auto &currency) {
        currency.supply += quantity;
    });
}
}; // namespace eosio

EOSIO_ABI(eosio::bond, (create)(issue)(distrtokens)(transfer)(transferid)(burn)(setrampayer)(addmilestone));
