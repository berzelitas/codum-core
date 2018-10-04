#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem {
class system_contract;
}

namespace eosio {

using std::string;

class bond : public contract {
public:
  bond(account_name self) : contract(self) {}

  [[eosio::action]]
  void create(account_name issuer, asset maximum_supply);

  [[eosio::action]]
  void issue(account_name to, asset quantity, string memo);

  [[eosio::action]]
  void transfer(account_name from, account_name to, asset quantity, string memo);

private:
  inline asset get_supply(symbol_name sym) const;

  inline asset get_balance(account_name owner, symbol_name sym) const;

  struct [[eosio::table]] account {
    asset balance;

    uint64_t primary_key() const { return balance.symbol.name(); }

    EOSLIB_SERIALIZE(account, (balance))
  };

  typedef eosio::multi_index<N(accounts), account> accounts;

  struct [[eosio::table]] stat {
    asset supply;
    asset max_supply;
    account_name issuer;
    string project;

    uint64_t primary_key() const { return supply.symbol.name(); }

    EOSLIB_SERIALIZE(stat, (supply)(max_supply)(issuer)(project))
  };

  typedef eosio::multi_index<N(stat), stat> stats;

  struct [[eosio::table]] schedule {
    uint64_t id;
    asset bond;
    uint8_t weight;
    time deadline;
    asset budget;
    asset token_budget;
    uint8_t funding_status;
    uint8_t execution_status;

    uint64_t primary_key() const { return id; }

    uint64_t get_bond_symbol() const { return bond.symbol.name(); }

    EOSLIB_SERIALIZE(schedule, (id)(bond)(weight)(deadline)(budget)(token_budget)(funding_status)(execution_status))
  };

  using schedules = eosio::multi_index<N(schedule), schedule,
          indexed_by < N(bond_symbol), const_mem_fun < schedule, uint64_t, &schedule::get_bond_symbol> > >;

  void sub_balance(account_name owner, asset value);

  void add_balance(account_name owner, asset value, account_name ram_payer);
};

asset bond::get_supply(symbol_name sym) const {
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);
    return st.supply;
}

asset bond::get_balance(account_name owner, symbol_name sym) const {
    accounts accountstable(_self, owner);
    const auto &ac = accountstable.get(sym);
    return ac.balance;
}

}; // namespace eosio
