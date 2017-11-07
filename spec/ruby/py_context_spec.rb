require_relative './spec_helper'

describe Rubython::PyContext do
  after(:each) do
    # Tear down the PyContext, if we created one
    Rubython::PyContext.teardown
  end

  it 'exists' do
    expect(Rubython::PyContext).not_to be_nil
  end

  describe 'PyContext instance' do
    it 'is initially un-initialized' do
      expect(Rubython::PyContext.instance).to be_nil
    end

    it 'is initialized upon first use' do
      context = Rubython::PyContext.new()
      expect(context).not_to be_nil
      expect(Rubython::PyContext.instance).not_to be_nil
    end

    it 'should be the same PyContext every time after initialization' do
      require 'pry'
      binding.pry
      context = Rubython::PyContext.new()
      expect(context).to be(Rubython::PyContext.instance)
      context2 = Rubython::PyContext.new()
      expect(context).to eq(context2)
    end

    it 'should invalidate any pre-existing references upon tearing down the instance' do
      context = Rubython::PyContext.new
      Rubython::PyContext.teardown
      expect(Rubython::PyContext.instance).to be_nil
      expect { context.local_variables }.to raise_error(RuntimeError)
    end
  end
end
