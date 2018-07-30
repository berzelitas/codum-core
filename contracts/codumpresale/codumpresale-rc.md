# Smart Contract - `{{ codumpresale }}`

This is an overview of the actions for the `{{ codumpresale }}` smart contract. 
This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ codumpresale }}` contract manages codum Fundraising/Token sale round A in EOSIO based network infrastructure.

### Actions, Inputs and Input Types

The table below contains the `actions`, `inputs` and `input types` for the `{{ codumpresale }}` contract.

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ apply }}` | `{{ contributor }}`<br/>`{{ acceptedterms }}` | `{{ name }}`<br/>`{{ bool }}` |
| `{{ approve }}` | `{{ contributor }}` | `{{ name }}` |
| `{{ buycodum }}` | `{{ contributor }}`<br/>`{{ network }}`<br/>`{{ quantity }}`<br/>`{{ memo }}` | `{{ name }}`<br/>`{{ uint8 }}`<br/>`{{ asset }}`<br/>`{{ string }}` |
| `{{ validate }}` | `{{ id }}`<br/>`{{ memo }}`<br/>`{{ transaction }}` | `{{ uint64 }}`<br/>`{{ string }}`<br/>`{{ string }}` |
| `{{ deletetx }}` | `{{ id }}` | `{{ uint64 }}` |
| `{{ distribute }}` | `{{ id }}` | `{{ uint64 }}` |
| `{{ refundsale }}` | `{{ id }}`<br/>`{{ refund_tx }}` | `{{ uint64 }}`<br/>`{{ string }}` |
| `{{ finish }}` | `{{ finish }}` | `{{ finish }}` |