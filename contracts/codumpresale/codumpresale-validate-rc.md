# Action - `{{ validate }}`

This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ validate }}` action validates the `contribution` with `{{ idVar }}` and sets if for distribution. It can only be executed by `codumpresale`. The `rate` will be updated from Token's `exrate` table and confirmed Token distribution allocation will be then set to `codum_dist`, together with bonus tokens allocated to `codum_bonus` if applicable. `{{ memoVar }}` must match the one set in the record for the action to succeed, and incoming transaction ID can be recorded if provided as `{{ transactionVar }}`. 

### Inputs and Input Types

The `{{ validate }}` action requires the following `inputs` and `input types`:

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ validate }}` | `{{ idVar }}`<br/>`{{ memoVar }}`<br/>`{{ transactionVar }}` | `{{ uint64 }}`<br/>`{{ string }}`<br/>`{{ string }}` |
