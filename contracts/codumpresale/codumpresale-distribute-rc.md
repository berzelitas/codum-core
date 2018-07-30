# Action - `{{ distribute }}`

This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ distribute }}` action allows `codumpresale` to distribute tokens managed by this contract by the conditions set in the `contribution` table for the specific record, which is determined by `{{ idVar }}` provided. This action is only possible after `softcap` is reached. The `{{ idVar }}` record also needs to be `validated`, not yet `distributed` and not yet `refunded`. Tokens will be executed until `hardcap`, after which `{{ idVar }}` contribution will be set for `refund`.

### Input and Input Type

The `{{ distribute }}` action requires the following `input` and `input type`:

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ distribute }}` | `{{ idVar }}` | `{{ uint64 }}` |
