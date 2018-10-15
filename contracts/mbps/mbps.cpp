#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

#include "mbps.hpp"

namespace eosio {

void bond::create(account_name issuer, asset maximum_supply) {
    require_auth(_self);

    // eliminating precision of symbol for NFT compatibility. Need to round asset.amount by precision
    symbol_type symbol = maximum_supply.symbol.name() << 8;
    uint16_t pres = pow(10, maximum_supply.symbol.precision());

    eosio_assert((maximum_supply.amount % pres) == 0, "maximum supply must be a whole number");
    eosio_assert(symbol.is_valid(), "invalid symbol name");
    eosio_assert(maximum_supply.is_valid(), "invalid supply");
    eosio_assert(maximum_supply.amount > 0, "max-supply must be positive");

    stats statstable(_self, symbol.name());
    auto existing = statstable.find(symbol.name());
    eosio_assert(existing == statstable.end(), "token with symbol already exists");

    statstable.emplace(_self, [&](auto &s) {
        s.supply.symbol = symbol;
        s.max_supply = asset(maximum_supply.amount / pres, symbol);
        s.issuer = issuer;
        s.project = "";
        s.token_price.symbol = symbol;
        s.budget_contract = 0;
        s.token_budget.symbol = symbol;
        s.tokens_in_bond.symbol = symbol;
        s.token_contract = 0;
        s.start = 0;
        s.fundraising_end = 0;
        s.end = 0;
    });
}

void bond::setupbond(
  asset bond,
  string project,
  asset token_price,
  account_name budget_contract,
  asset token_budget,
  asset tokens_in_bond,
  account_name token_contract,
  time start,
  time fundraising_end,
  time end
) {
    eosio_assert(start > now() && fundraising_end > now() && end > now(),
                 "project timeframe must be in the future if edited");
    eosio_assert(start < fundraising_end && fundraising_end <= end,
                 "the project must start before the end of fundraising, which needs to be before the end of the project");

    eosio_assert(token_price.is_valid(), "token price is not a valid asset");
    eosio_assert(token_price.amount >= 0, "token price must be positive");

    eosio_assert(token_budget.is_valid(), "token budget is not a valid asset");
    eosio_assert(token_budget.amount >= 0, "token budget must be positive");

    eosio_assert(is_account(token_contract), "account does not exist");

    //Searching token_price in budget_contract
    account_index price_account(budget_contract, token_price.symbol.name());
    auto account_itr = price_account.find(token_price.symbol.name());
    eosio_assert(account_itr != price_account.end(), "token_price does not exist in budget_contract");

    account_index budget_account(token_contract, token_budget.symbol.name());
    account_itr = budget_account.find(token_budget.symbol.name());
    eosio_assert(account_itr != budget_account.end(), "token_budget does not exist in token_contract");

    stats statstable(_self, bond.symbol.name());
    auto stat_itr = statstable.find(bond.symbol.name());
    eosio_assert(stat_itr != statstable.end(), "there is no stat with bond");
    eosio_assert(stat_itr->supply.amount > 0, "at least one bon already issued");

    statstable.modify(stat_itr, _self, [&](auto &s) {
        s.project = project;
        s.token_price = token_price;
        s.budget_contract = budget_contract;
        s.token_budget = token_budget;
        s.tokens_in_bond = tokens_in_bond;
        s.token_contract = token_contract;
        s.start = start;
        s.fundraising_end = fundraising_end;
        s.end = end;
    });
}

void bond::issue(account_name to, asset quantity, string name, string memo) {
    eosio_assert(is_account(to), "to account does not exist");

    // e,g, Get EOS from 3 EOS
    // eliminating precision of symbol for NFT compatibility. Need to round asset.amount by precision
    symbol_type symbol = quantity.symbol.name() << 8;
    uint16_t pres = pow(10, quantity.symbol.precision());
    eosio_assert((quantity.amount % pres) == 0, "quantity must be a whole number");
    quantity.symbol = symbol;
    quantity.amount = quantity.amount / pres;

    eosio_assert(symbol.is_valid(), "invalid symbol name");
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    eosio_assert(name.size() <= 32, "name has more than 32 bytes");
    eosio_assert(name.size() > 0, "name is empty");

    // Ensure currency has been created
    auto symbol_name = symbol.name();
    stats currency_table(_self, symbol_name);
    auto existing_currency = currency_table.find(symbol_name);
    eosio_assert(existing_currency != currency_table.end(),
                 "token with symbol does not exist. create token before issue");
    const auto &st = *existing_currency;

    // Ensure have issuer authorization and valid quantity
    require_auth(st.issuer);
    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(quantity.amount > 0, "must issue positive quantity");
    eosio_assert(symbol == st.supply.symbol, "symbol precision mismatch");

    // Increase supply
    add_supply(quantity);

    // Mint nfts
    eosio::print("amount = ", quantity.amount);
    for (uint8_t i = 0; i < quantity.amount; i++) {
        mint(to, st.issuer, asset{1, symbol}, name);
    }

    // Add balance to account
    add_balance(to, quantity, st.issuer);
}

void bond::distrtokens(id_type token_id) {
    // token bondtoken = tokens.get(token_id);
    // eosio_assert(bondtoken.id == token_id, "token with id not found");

    // auto sym = bondtoken.value.symbol;

    // currency_index stat(_self, _self);
    // stats bs = stat.get(sym);
    // eosio_assert(bs.supply.value.symbol == sym, "stat with token symbol not found");

    // require_auth(bs.issuer);

    // std::vector<eosio::bond::schedule> ms;
    // schedule_index schedules(_self, _self);
    // auto symbl = schedules.get_index<N(bysymbol)>();

    // for(auto itr = symbl.begin(); itr != symbl.end(); itr++) {
    //     if(itr->bond.symbol.name == sym) {
    //         ms.push_back();
    //     };
    // };
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

void bond::transfer(account_name from, account_name to, asset quantity, string memo) {
    // Ensure authorized to send from account
    eosio_assert(from != to, "cannot transfer to self");
    require_auth(from);

    // Ensure 'to' account exists
    eosio_assert(is_account(to), "to account does not exist");

    // eliminating precision of symbol for NFT compatibility. Need to round asset.amount by precision
    symbol_type symbol = quantity.symbol.name() << 8;
    uint16_t pres = pow(10, quantity.symbol.precision());
    eosio_assert((quantity.amount % pres) == 0, "quantity must be a whole number");
    quantity.symbol = symbol;
    quantity.amount = quantity.amount / pres;

    // Check memo size and print
    eosio_assert(memo.size() <= 256, "memo has more than 256 bytes");

    eosio_assert(quantity.amount == 1, "cannot transfer quantity, not equal to 1");

    auto symbl = tokens.get_index<N(bysymbol)>();

    bool found = false;
    id_type id = 0;
    for (auto it = symbl.begin(); it != symbl.end(); ++it) {

        if (it->value.symbol == quantity.symbol && it->owner == from) {
            id = it->id;
            found = true;
            break;
        }
    }

    eosio_assert(found, "token is not found or is not owned by account");

    // Notify both recipients
    require_recipient(from);
    require_recipient(to);

    SEND_INLINE_ACTION(*this, transferid, {from, N(active)}, {from, to, id, memo});
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
        token.owner = st.owner;
        token.value = st.value;
        token.name = st.name;
    });

    sub_balance(payer, st.value);
    add_balance(payer, st.value, payer);
}

void bond::mint(account_name owner, account_name ram_payer, asset value, string name) {
    // Add token with creator paying for RAM
    tokens.emplace(ram_payer, [&](auto &token) {
        token.id = tokens.available_primary_key();
        token.owner = owner;
        token.value = value;
        token.name = name;
    });
}

void bond::addmilestone(
  asset bond,
  uint32_t weight,
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
    require_auth(bs->token_contract);

    eosio_assert(deadline > bs->start && deadline < bs->end, "The deadline must be between of start and end.");

    eosio_assert(milestone.length() >= 16, "The milestone length must be at least 16 characters.");

    eosio_assert(bond.amount == 0, "Bond quantity must be equal to 0.");

    eosio_assert(weight <= 1000000, "Total weight of all bond.symbol related milestones must not exceed 1000000.");

    account_index from_budget_acnts(bs->budget_contract, _self);
    const auto &budget_acc = from_budget_acnts.get(budget.symbol.name());
    eosio_assert(budget_acc.balance.amount >= 0, "Budget must be a valid asset symbol with 0 or positive quantity.");

    account_index from_token_acnts(bs->token_contract, _self);
    const auto &token_acc = from_token_acnts.get(token_budget.symbol.name());
    eosio_assert(token_acc.balance.amount >= 0,
                 "Token budget must be a valid asset symbol with 0 or positive quantity.");

    schedule_index schedulable(_self, bond.symbol.name());
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
  uint32_t weight,
  time deadline,
  asset budget,
  uint8_t execution_status,
  string update_message,
  string press_release_url
) {
    schedule_index schedulable(_self, budget.symbol.name());
    const auto &m = schedulable.get(id, "No schedule object found.");

    stats statstable(_self, budget.symbol.name());
    auto bs = statstable.find(m.bond.symbol.name());

    eosio_assert(bs != statstable.end(), "Non-existed stats for bond.");

    require_auth(bs->token_contract);

    eosio_assert(
      m.execution_status < 0 || m.execution_status >= 100 || (execution_status >= 0 && execution_status <= 100),
      "Wrong execution status.");

    eosio_assert(m.execution_status >= 100 && m.execution_status == execution_status,
                 "Cannot update milestone execution status which is submitted for completion.");

    bool last_deadline = m.funding_status == 0;
    if (last_deadline) {
        for (auto& itr : schedulable) {
           if (itr.id != id && itr.execution_status < 100 && itr.deadline > m.deadline) {
                last_deadline = false;
                break;
            }
        }
    }

    if (m.execution_status == 100) {
        eosio_assert(m.budget == budget, "Argument 'budget' is forbidden.");
        eosio_assert(m.deadline == deadline, "Argument 'deadline' is forbidden.");
        eosio_assert(m.weight == weight, "Argument 'weight' is forbidden.");
    }

    schedulable.modify(m, _self, [&](auto &mt) {
        mt.deadline = deadline;

        if (last_deadline) {
            mt.weight = weight;
            mt.budget = budget;
        }

        if (execution_status != 100) {
            mt.execution_status = execution_status;
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

uint16_t bond::pow(uint16_t a, uint16_t b) {
    uint16_t res = 1;
    for (uint8_t i = 0; i < b; i++) {
        res *= a;
    }
    return res;
}
}; // namespace eosio

EOSIO_ABI(
  eosio::bond,
  (create)(setupbond)(issue)(distrtokens)(transfer)(transferid)(burn)(setrampayer)(addmilestone)(updatemilest)
);
