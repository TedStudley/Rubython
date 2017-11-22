require 'bundler'
Bundler.setup

require 'rspec'
require 'rubython'
require 'support/matchers'

PYTHON_HELPER_PATH = File.expand_path('../support/python_helper.py', __FILE__)

include Rubython
include Rubython::PyTypes

RSpec.configure do |config|
  config.include Rubython::Spec::Matchers
end
