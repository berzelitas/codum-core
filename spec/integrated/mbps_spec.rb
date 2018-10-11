RSpec.describe 'Bond contract' do
  describe '#create' do
    it 'throws assert(maximum supply must be a whole number)' do
      result = push_action( "create", ["foo", "1000.12 FOO"], 'test' )

      expect(result.stdout).to eq ''
      expect(result.stderr).to match /maximum supply must be a whole number/
    end
    it 'throws assert(token with symbol already exists)' do
      push_action( "create", ["bar", "1000.00 BAR"], 'test' )
      sleep 1
      result = push_action( "create", ["bar", "1.00 BAR"], 'test' )

      expect(result.stdout).to eq ''
      expect(result.stderr).to match /token with symbol already exists/
    end
    it 'token with symbol FOO was created succesful' do
      push_action( "create", ["foo", "1000.00 FOO"], 'test' )
      result = get_table( "test", "FOO", "stat" )

      expect(result.stdout).to match (
      /{
        "rows": [{
            "supply": "0 FOO",
            "max_supply": "1000 FOO",
            "issuer": "foo",
            "project": "",
            "token_price": "0 FOO",
            "budget_contract": "",
            "token_budget": "0 FOO",
            "tokens_in_bond": "0 FOO",
            "token_contract": "",
            "start": 0,
            "fundraising_end": 0,
            "project": 0
          }
        ],
        "more": false
      }/)
    end
  end
end


# { 
#   "rows": [{
#       "supply": "0 FOO",
#       "max_supply": "1000 FOO",
#       "issuer": "foo",
#       "project": "",
#       "token_price": "0 FOO",
#       "budget_contract": "",
#       "token_budget": "0 FOO",
#       "tokens_in_bond": "0 FOO",
#       "token_contract": "",
#       "start": 0,
#       "fundraising_end": 0,
#       "project": 0
#     }
#   ],
#   "more": false
# }
