# Smart Contract - `{{ codum.token }}`

This is an overview of the actions for the `{{ codum.token }}` smart contract. 
This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ codum.token }}` contract is the contract enabling CODUM token functionality in EOSIO based network infrastructure.

### Actions, Inputs and Input Types

The table below contains the `actions`, `inputs` and `input types` for the `{{ codum.token }}` contract.

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ create }}` | `{{ issuer }}`<br/>`{{ maximum_supply }}` | `{{ name }}`<br/>`{{ asset }}` |
| `{{ issue }}` | `{{ to }}`<br/>`{{ quantity }}`<br/>`{{ memo }}` | `{{ name }}`<br/>`{{ asset }}`<br/>`{{ string }}` |
| `{{ transfer }}` | `{{ from }}`<br/>`{{ to }}`<br/>`{{ quantity }}`<br/>`{{ memo }}` | `{{ name }}`<br/>`{{ name }}`<br/>`{{ asset }}`<br/>`{{ string }}` |
| `{{ setgrunlock }}` | `{{ date }}`<br/>`{{ percent }}` | `{{ uint64 }}`<br/>`{{ uint8 }}` |
| `{{ launchlock }}` | `{{ to }}`<br/>`{{ quantity }}` | `{{ name }}`<br/>`{{ asset }}` |
| `{{ gradlock }}` | `{{ to }}`<br/>`{{ quantity }}` | `{{ name }}`<br/>`{{ asset }}` |
| `{{ distribsale }}` | `{{ from }}`<br/>`{{ to }}`<br/>`{{ quantity }}`<br/>`{{ memo }}` | `{{ name }}`<br/>`{{ name }}`<br/>`{{ asset }}`<br/>`{{ string }}` |
| `{{ distribcontr }}` | `{{ from }}`<br/>`{{ to }}`<br/>`{{ quantity }}`<br/>`{{ memo }}` | `{{ name }}`<br/>`{{ name }}`<br/>`{{ asset }}`<br/>`{{ string }}` |
| `{{ setdistrib }}` | `{{ currency }}`<br/>`{{ distributor }}` | `{{ asset }}`<br/>`{{ name }}` |
| `{{ updaterate }}` | `{{ network }}`<br/>`{{ rate }}` | `{{ uint8 }}`<br/>`{{ uint32 }}` |