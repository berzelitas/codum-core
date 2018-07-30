# Action - `{{ deletetx }}`

This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ deletetx }}` action allows `codumpresale` to delete unconfirmed Token acquisition intent from `contribution` table when `{{ idVar }}` is provided.

### Input and Input Type

The `{{ deletetx }}` action requires the following `input` and `input type`:

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ deletetx }}` | `{{ idVar }}` | `{{ uint64 }}` |
