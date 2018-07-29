# Action - `{{ updaterate }}`

This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ updaterate }}` action sets the network exchange rate for all tokens managed by this smart contract.
`{{ networkVar }}` determines the network protocol and token rate `{{ rateVar }}` being set for the current time being against the Tokens managed by this contract. It is assumed that networks are: 0 = EOS, 1 = EUR, 2 = USD, 3 = ETH.


### Inputs and Input Types

The `{{ updaterate }}` action requires the following `inputs` and `input types`:

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ updaterate }}` | `{{ networkVar }}`<br/>`{{ rateVar }}` | `{{ uint8 }}`<br/>`{{ uint32 }}` |
