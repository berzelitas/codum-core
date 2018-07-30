# Action - `{{ buycodum }}`

This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ buycodum }}` action submits the `{{ contributorVar }}`'s intent of ackquisition of Tokens managed by this contract. `{{ networkVar }}` must be set between 0 and 3, where 0 = EOS, 1 = EUR, 2 = USD and 3 = ETH. `{{ quantityVar }}` must be provided with 4 precision number, for example "100.0000 EOS" or "1000.0000 EUR", where symbol must match the `{{ networkVar }}`. `{{ memoVar }}` is also required and preferred to be unique. In order to guarantee uniquiness, it is recommended to execute buycodum through official CODUM fundraising website while logged in with Scatter at https://codum.io/fundraising/. The intent information is then being saved into `contribution` table as a record, where the preliminary network exchange rate and estimated Token allocation to `codum_dist` will also be set on the intent record. 
Note: `{{ contributorVar }}` account can only have up to 3 outstanding Token ackquisition intention records at any given point of time before they are validated. Intent record gets validated once `codumpresale` managers process the incoming funds for the Tokens.

### Inputs and Input Types

The `{{ buycodum }}` action requires the following `inputs` and `input types`:

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ buycodum }}` | `{{ contributorVar }}`<br/>`{{ networkVar }}`<br/>`{{ quantityVar }}`<br/>`{{ memoVar }}` | `{{ name }}`<br/>`{{ uint8 }}`<br/>`{{ asset }}`<br/>`{{ string }}` |
