require 'rspec/expectations'

module Rubython
  module Spec
    module Matchers
      RSpec::Matchers.define :contain do |expected|
        match do |actual|
          actual.__contains__(expected)
        end
        failure_message do |actual|
          "expected #{expected} in #{actual} (via Python __contains__)"
        end
      end
    end
  end
end
