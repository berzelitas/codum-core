# Action - `{{ finish }}`

This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ finish }}` action can only be executed by `codumpresale` and sets this Fundraising/Token sale round A to be complete. It can only be executed once `hardcap` is reached or sale `end`s. If `softcap` is not reached after `end` date and time, all `contribution` records are set for refund. The refunds will be processed afterwards manually. Otherwise, if `softcap` has been reached, this action cannot be executed until not `distributed` or not `refunded` but `validated` transactions exist in the `contribution` table.

### Input and Input Type

The `{{ finish }}` action requires the following `input` and `input type`:

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ finish }}` | `{{ finishVar }}` | `{{ finish }}` |
