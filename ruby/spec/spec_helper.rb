require 'bundler'
Bundler.setup

require 'rspec'
require 'rubython'
require 'support/matchers'

RSpec.configure do |config|
  config.include Rubython::Spec::Matchers
end
