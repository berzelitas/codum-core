# Action - `{{ refundsale }}`

This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ refundsale }}` action confirms refund for `contribution` with `{{ id }}` `{{ idVar }}`. This action can only be executed by `codumpresale` and requires `contribution` record to be `validated` and not yet `refunded`. In case the `network` is 0 (EOS), the EOS transfer transaction with `quantity` is executed. Otherwise, `{{ refund_txVar }}` is required to be provided.

### Inputs and Input Types

The `{{ refundsale }}` action requires the following `inputs` and `input types`:

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ refundsale }}` | `{{ idVar }}`<br/>`{{ refund_txVar }}` | `{{ uint64 }}`<br/>`{{ string }}` |
