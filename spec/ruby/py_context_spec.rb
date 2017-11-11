require_relative './spec_helper'

EXCEPTION_CASES = [
  ['foo', 'name \'foo\' is not defined'],
  ['1 + "foo"', 'unsupported operand type(s)'],
  ['1 / 0', 'integer division or modulo by zero']
]

include Rubython
describe PyContext do
  after(:each) do
    # Tear down the PyContext, if we created one
    PyContext.teardown
  end

  it 'exists' do
    expect(PyContext).not_to be_nil
  end

  it 'should be initializable' do
    context = PyContext.new
    expect(context).not_to be_nil
  end

  it 'should not be initialized before first use' do
    expect(PyContext.get_instance).to be_nil
    context = PyContext.new
    exepct(PyContext.get_instance).not_to be_nil
  end

  it 'should share the same instance between all wrappers' do
    context_a = PyContext.new
    context_b = PyContext.new
    expect(context_a).to eq(context_b)
    expect(context_a).to eq(PyContext.get_instance)
  end

  it 'should be able to be torn down' do
    context = PyContext.new
    expect(PyContext.get_instance).not_to be_nil
    PyContext.finalize!
    expect(PyContext.get_instance).to be_nil
  end

  it 'should raise an exception when attempting to reference a finalized context' do
    context = PyContext.new
    PyContext.finalize!
    expect { context.global_variables }.to raise_error(RuntimeError)
  end

  it 'should allow creating a new context after finalizing a previous one' do
    context_a = PyContext.new
    PyContext.finalize!
    context_b = PyContext.new
    expect(context_a).not_to eq(context_b)
    expect(context_b).to eq(PyContext.get_instance)
  end

  it 'should be able to influence values in Python' do
    context = PyContext.new
    context.py_exec('foo = "bar"')
    # We should have a new local variable
    expect(context.py_eval('foo').to_s).to eq('bar')
  end

  it 'should be able to list the local variables in a context' do
    context = PyContext.new
    expect(context.local_variables.inspect).to eq('{}')
  end

  it 'should be able to list the global variables in a context' do
    context = PyContext.new
    expect(context.global_variables.inspect).to eq('{}')
  end

  it 'should properly refresh the context state between context instances' do
    context_a = PyContext.new
    context_a.py_exec('foo = "bar"')
    expect(context_a.local_variables).to include('foo')
    PyContext.finalize!
    context_b = PyContext.new
    expect(context_b.local_variables).not_to include('foo')
    expect { context_b.py_eval('foo') }.to raise(RuntimeError)
  end

  it 'should be able to manipulate values within Ruby' do
    context = PyContext.new
    context.py_exec('baz = "foo"')
    context.py_exec('qux = "bar"')
    expect(context.py_eval('baz + qux')).to eq('foobar')
  end

  it 'should raise exceptions with messages forwarded from Python' do
    context = PyContext.new
    EXCEPTION_CASES.each do |eval_str, expected_message|
      expect { context.py_eval(eval_str) }.to raise(RuntimeError, expected_message)
    end
  end

  it 'should be able to modify variables from the Ruby side' do
    context = PyContext.new
    context.py_exec("foobar = 'foo'")
    context.py_exec("foobar += 'bar'")
    expect(context.py_eval('foobar')).to eq('foobar')
  end
end
