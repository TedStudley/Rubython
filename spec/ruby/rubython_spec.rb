require_relative './spec_helper'

describe Rubython do
  it 'exists' do
    expect(Rubython::VERSION).not_to be_nil
  end

  it 'provides the necessary PyContext type' do
    expect(Rubython::PyContext).not_to be_nil
  end
end
