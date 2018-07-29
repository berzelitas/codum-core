# Action - `{{ setdistrib }}`

This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ setdistrib }}` action delegates the distributor account right to `{{ distributorVar }}` for `{{ currencyVar }}` Token. The numerical value provided in `{{ currencyVar }}` is insignificant and is being ignored, only the Token symbol is taken into account.

### Inputs and Input Types

The `{{ setdistrib }}` action requires the following `inputs` and `input types`:

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ setdistrib }}` | `{{ currencyVar }}`<br/>`{{ distributorVar }}` | `{{ asset }}`<br/>`{{ name }}` |
