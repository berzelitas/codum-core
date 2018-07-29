# Action - `{{ setgrunlock }}`

This Contract is legally binding and can be used in the event of a dispute.
Disputes shall be settled through the Lithuanian court of Commercial Arbitration as set in CODUM TOKEN ACQUISITION AGREEMENT.
By interacting with this contract you unconditionally agree with CODUM TOKEN ACQUISITION AGREEMENT. 
CODUM TOKEN ACQUISITION AGREEMENT can be found at https://codum.io/fundraising/terms/.

### Description

The `{{ setgrunlock }}` action sets the gradual unlock clause in `gradunlock` table for all tokens distributed through this contract. 

### Inputs and Input Types

The `{{ setgrunlock }}` action requires the following `inputs` and `input types`:

| Action | Input | Input Type |
|:--|:--|:--|
| `{{ setgrunlock }}` | `{{ dateVar }}`<br/>`{{ percentVar }}` | `{{ uint64 }}`<br/>`{{ uint8 }}` |
