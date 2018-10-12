# Require to start nodeos with --verbose-http-errors
# Some tests can fail with "transaction took too long". Just
RSpec.describe 'Bond contract' do
  describe '#create' do
    it '1.1 throws assert(maximum supply must be a whole number 0.12 FOO)' do
      result = push_action( "create", ["foo", "0.12 FOO"], 'test' )

      expect(result.stdout).to eq ''
      expect(result.stderr).to match /maximum supply must be a whole number/
    end
    it '1.2 throws assert(maximum supply must be a whole number 1000.12 FOO)' do
      result = push_action( "create", ["foo", "1000.12 FOO"], 'test' )

      expect(result.stdout).to eq ''
      expect(result.stderr).to match /maximum supply must be a whole number/
    end
    it '1.3 token with symbol FOO was created succesful' do
      peremennaya1 = push_action( "create", ["foo", "1000.00 FOO"], 'test' )
      result = get_table( "test", "FOO", "stat" )
      
      expect(result.stderr).to eq ''
      output = JSON.parse result.stdout
      expect(output['rows']).to be_a(Array)
      expect(output['rows'].first).to include (
        {
          "supply"=> "0 FOO",
          "max_supply"=> "1000 FOO",
          "issuer"=> "foo",
          "project"=> "",
          "token_price"=> "0 FOO",
          "budget_contract"=> "",
          "token_budget"=> "0 FOO",
          "tokens_in_bond"=> "0 FOO",
          "token_contract"=> "",
          "start"=> 0,
          "fundraising_end"=> 0,
          "project"=> 0
        })
    end
    it '1.4 throws assert(token with symbol already exists)' do
      result = push_action( "create", ["foo", "1.00 FOO"], 'test' )

      expect(result.stdout).to eq ''
      expect(result.stderr).to match /token with symbol already exists/
    end
  end
  describe '#setupbond' do
  # is not ready yet. Skipping
  end
  describe '#issue' do
    it '3.1 throws assert(quantity must be a whole number 1.10 FOO)' do
      result = push_action( "issue", ["bar", "1.10 FOO", "foo_token", "issued 1 FOO"], 'foo' )

      expect(result.stdout).to eq ''
      expect(result.stderr).to match /quantity must be a whole number/
    end
    it '3.2 throws assert(token with symbol does not exist. create token before issue)' do
      result = push_action( "issue", ["bar", "1.00 FOOBAR", "test", "issued 1 FOOBAR"], 'foo' )

      expect(result.stdout).to eq ''
      expect(result.stderr).to match /token with symbol does not exist. create token before issue/
    end
    # it '3.3 throws assert(token with symbol does not exist. create token before issue)' do
    #   result = push_action( "issue", ["bar", "1.00 FOOBAR", "test", "issued 1 FOOBAR"], 'foo' )

    #   expect(result.stdout).to eq ''
    #   expect(result.stderr).to match /token with symbol does not exist. create token before issue/
    # end
    # it '3.4 throws assert(token with symbol does not exist. create token before issue)' do
    #   result = push_action( "issue", ["bar", "1.00 FOO", "test", "issued 1 FOOBAR"], 'foo' )

    #   expect(result.stdout).to eq ''
    #   expect(result.stderr).to match /token with symbol does not exist. create token before issue/
    # end
  end
end