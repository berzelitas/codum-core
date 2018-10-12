require 'json'
require 'open3'
require 'ostruct'

# cleos wrapper for tests
module Cleos
  # Exec command
  #
  # @param  [Array|String] cmd
  # @return [OpenStruct]
  def command(cmd)
    cmd = cmd.join(' ') if cmd.is_a? Array

    stdout, stderr, status = Open3.capture3 cmd
    OpenStruct.new stdout: stdout, stderr: stderr, status: status.to_i
  end

  # Exec cleos command
  #
  # @param  [Array|String] cmd
  # @return [OpenStruct]
  def cleos_command(cmd)
    command cmd.is_a?(Array) ? [cleos_cli, *cmd] : cleos_cli + ' ' + cmd
  end

  # Push action by cleos to eos
  #
  # @param  [Symbol|String] action_name
  # @param  [Hash|Array]    params
  # @return [OpenStruct]
  def push_action(action_name, params, user = nil)
    user ||= ENV.fetch('CONTRACT_USER', 'eosio')

    cleos_command [
      'push',
      'action',
      ENV.fetch('CONTRACT_NAME', 'test'),
      action_name.to_s,
      "'#{params.to_json}'",
      '-p',
      user
    ]
  end

  def get_table(contract_name, scope, table)

    cleos_command [
      'get',
      'table',
      contract_name,
      scope,
      table
    ]
  end

  private

  def cleos_cli
    'cleos'
  end
end
