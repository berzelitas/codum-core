RSpec.describe 'Bond contract' do
  describe '#create' do
    it 'aserrts message for maximum_supply with wrong amount' do
      result = push_action :create, [1, 2]

      expect(result.stdout).to eq ''
      expect(result.stderr.length).to be_positive
      expect(result.status).to be 32512
    end
  end
end
