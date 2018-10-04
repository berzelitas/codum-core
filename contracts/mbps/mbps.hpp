#pragma once

#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>

#include <string>

namespace eosiosystem {
class system_contract;
}

namespace eosio {
using std::string;
typedef uint128_t uuid;
typedef uint64_t id_type;
typedef string uri_type;

class bond : public contract {
public:
  bond(account_name self) : contract(self), tokens(_self, _self) {}

  [[eosio::action]]
  void create(account_name issuer, asset maximum_supply);

  [[eosio::action]]
  void issue(account_name to, asset quantity, string memo);

  [[eosio::action]]
  void distrtokens(id_type token_id);

  [[eosio::action]]
  void transfer(account_name from, account_name to, asset quantity, string memo);

  [[eosio::action]]
  void transferid(account_name from, account_name to, id_type id, string memo);

  [[eosio::action]]
  void burn(account_name owner, id_type token_id);

  [[eosio::action]]
  void setrampayer(account_name payer, id_type id);

  [[eosio::action]]
  void addmilestone(
    asset bond,
    uint16_t weight,
    time deadline,
    asset budget,
    asset token_budget,
    string milestone,
    string press_release_url,
    string epic_url
  );

  struct [[eosio::table]] account {
    asset balance;

    uint64_t primary_key() const { return balance.symbol.name(); }
  };

  struct [[eosio::table]] stat {
    asset supply;
    asset max_supply;
    account_name issuer;
    string project;

    uint64_t primary_key() const { return supply.symbol.name(); }
  };

  struct [[eosio::table]] token {
    id_type id;          // Unique 64 bit identifier
    uri_type uri;        // RFC 3986
    account_name owner;  // token owner
    asset value;         // token value (1 SYS)
    string name;     // token name

    id_type primary_key() const { return id; }

    account_name get_owner() const { return owner; }

    string get_uri() const { return uri; }

    asset get_value() const { return value; }

    uint64_t get_symbol() const { return value.symbol.name(); }

    uint64_t get_name() const { return string_to_name(name.c_str()); }

    uuid get_global_id() const {
        uint128_t self_128 = static_cast<uint128_t>(N(_self));
        uint128_t id_128 = static_cast<uint128_t>(id);
        uint128_t res = (self_128 << 64) | (id_128);
        return res;
    }

    string get_unique_name() const {
        string unique_name = name + "#" + std::to_string(id);
        return unique_name;
    }
  };

  struct [[eosio::table]] schedule {
    id_type id;
    asset bond;
    uint8_t weight;
    time deadline;
    asset budget;
    asset token_budget;
    uint8_t funding_status;
    uint8_t execution_status;

    uint64_t primary_key() const { return id; }

    uint64_t get_bond_symbol() const { return bond.symbol.name(); }
  };

  struct [[eosio::table]] milestoneclaims {
    id_type id;
    id_type token_id;
    id_type milestone_id;

    uint64_t primary_key() const { return id; };

    uint64_t get_token_id() const { return token_id; };

    uint64_t get_milestone_id() const { return milestone_id; };
  };

  using account_index = eosio::multi_index<N(accounts), account>;

  using stats = eosio::multi_index<N(stat), stat>;

  using token_index = eosio::multi_index<N(token), token,
    indexed_by< N(byowner), const_mem_fun < token, account_name, &token::get_owner> >,
    indexed_by< N(bysymbol), const_mem_fun < token, uint64_t, &token::get_symbol> >,
    indexed_by< N(byname), const_mem_fun < token, uint64_t, &token::get_name> > >;

  using schedules = eosio::multi_index<N(schedule), schedule,
    indexed_by< N(bond_symbol), const_mem_fun < schedule, uint64_t, &schedule::get_bond_symbol> > >;

  using milestoneclaims_index = eosio::multi_index<N(milestone), milestoneclaims,
    indexed_by< N(bytoken), eosio::const_mem_fun<milestoneclaims, uint64_t, &milestoneclaims::get_token_id> >,
    indexed_by< N(bymilestone), eosio::const_mem_fun<milestoneclaims, uint64_t, &milestoneclaims::get_milestone_id> > >;


private:
  friend eosiosystem::system_contract;

  token_index tokens;

  // PRIVATE UTILITY FUNCTIONS
  void mint(account_name owner, account_name ram_payer, asset value, string uri, string name);

  void sub_balance(account_name owner, asset value);

  void add_balance(account_name owner, asset value, account_name ram_payer);

  void sub_supply(asset quantity);

  void add_supply(asset quantity);

  inline asset get_supply(symbol_name sym) const;

  inline asset get_balance(account_name owner, symbol_name sym) const;
};

asset bond::get_supply(symbol_name sym) const {
    stats statstable(_self, sym);
    const auto &st = statstable.get(sym);
    return st.supply;
}

asset bond::get_balance(account_name owner, symbol_name sym) const {
    account_index accountstable(_self, owner);
    const auto &ac = accountstable.get(sym);
    return ac.balance;
}

}; // namespace eosio
