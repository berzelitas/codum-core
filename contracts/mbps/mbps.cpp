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

void bond::transferid( account_name from, account_name to, id_type id, string memo ) {
    // Ensure authorized to send from account
    eosio_assert( from != to, "cannot transfer to self" );
    require_auth( from );

    // Ensure 'to' account exists
    eosio_assert( is_account( to ), "to account does not exist");

	// Check memo size and print
    eosio_assert( memo.size() <= 256, "memo has more than 256 bytes" );

    // Ensure token ID exists
    auto sender_token = tokens.find( id );
    eosio_assert( sender_token != tokens.end(), "token with specified ID does not exist" );

	// Ensure owner owns token
    eosio_assert( sender_token->owner == from, "sender does not own token with specified ID");

	const auto& st = *sender_token;

	// Notify both recipients
    require_recipient( from );
    require_recipient( to );

    // Transfer NFT from sender to receiver
    tokens.modify( st, from, [&]( auto& token ) {
	    token.owner = to;
    });

    // Change balance of both accounts
    sub_balance( from, st.value );
    add_balance( to, st.value, from );
}

void bond::burn( account_name owner, id_type token_id )
{
    require_auth( owner );

    // Find token to burn
    auto burn_token = tokens.find( token_id );
	eosio_assert( burn_token != tokens.end(), "token with id does not exist" );
	eosio_assert( burn_token->owner == owner, "token not owned by account" );

	asset burnt_supply = burn_token->value;

	// Remove token from tokens table
    tokens.erase( burn_token );

    // Lower balance from owner
    sub_balance( owner, burnt_supply );

    // Lower supply from currency
    sub_supply( burnt_supply );
}

void bond::setrampayer(account_name payer, id_type id) {
	require_auth(payer);

	// Ensure token ID exists
	auto payer_token = tokens.find( id );
	eosio_assert( payer_token != tokens.end(), "token with specified ID does not exist" );

	// Ensure payer owns token
	eosio_assert( payer_token->owner == payer, "payer does not own token with specified ID");

	const auto& st = *payer_token;

	// Notify payer
	require_recipient( payer );

	// Set owner as a RAM payer
	tokens.modify(payer_token, payer, [&](auto& token){
		token.id = st.id;
		token.uri = st.uri;
		token.owner = st.owner;
		token.value = st.value;
		token.name = st.name;
	});

	sub_balance( payer, st.value );
	add_balance( payer, st.value, payer );
}

void bond::mint( account_name owner, account_name ram_payer, asset value, string uri, string name)
{
    // Add token with creator paying for RAM
    tokens.emplace( ram_payer, [&]( auto& token ) {
        token.id = tokens.available_primary_key();
        token.uri = uri;
        token.owner = owner;
        token.value = value;
	    token.name = name;
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

void bond::sub_supply( asset quantity ) {
    auto symbol_name = quantity.symbol.name();
    stats currency_table( _self, symbol_name );
    auto current_currency = currency_table.find( symbol_name );

    currency_table.modify( current_currency, 0, [&]( auto& currency ) {
        currency.supply -= quantity;
    });
}

void bond::add_supply( asset quantity ) {
    auto symbol_name = quantity.symbol.name();
    stats currency_table( _self, symbol_name );
    auto current_currency = currency_table.find( symbol_name );

    currency_table.modify( current_currency, 0, [&]( auto& currency ) {
        currency.supply += quantity;
    });
}
}; // namespace eosio

EOSIO_ABI(eosio::bond, (create)(issue)(transfer));