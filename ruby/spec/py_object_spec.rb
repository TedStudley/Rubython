require_relative './spec_helper'

include Rubython
include Rubython::PyTypes
describe PyObject do
  before(:each) do
    @context = PyContext.new(PYTHON_HELPER_PATH)
    @PythonHelper = @context.py_eval("PythonHelper")
  end

  after(:each) do
    PyContext.finalize!
  end

  it 'exists' do
    expect(PyObject).to be
  end

  it 'should be possible to exfiltrate a PyObject reference from Python via the PyContext' do
    py_object = @context.py_eval('object()')
    expect(py_object).to be_an(PyObject)
  end

  it 'should be possible to call methods from Python with no arguments on a PyObject' do
    py_object = @context.py_eval('"foo"')
    expect(py_object.upper()).to eq('FOO')
  end

  it 'should be possible to call methods from Python with arguments on a PyObject' do
    py_object = @context.py_eval('[]')
    py_object.append(42)
    expect(py_object).to eq([42])
  end

  it 'should be possible to affect the Python context with Python methods called on an PyObject' do
    @context.py_exec('foo = []')
    py_object = @context.py_eval('foo')
    py_object.append(42)
    result = @context.py_eval('foo')
    expect(result).to eq([42])
  end

  it 'should provide a proper reference to an object in Python' do
    @context.py_exec('foo = []')
    py_object = @context.py_eval('foo')
    expect(py_object).to eq([])
    @context.py_eval('foo.append(42)')
    expect(py_object).to eq([42])
  end

  it 'should pass Ruby objects to Python with RbObject wrappers' do
    test_object = Object.new
    python_type = @PythonHelper.get_pytype(test_object)
    expect(python_type.to_s).to eq("<type 'rubython.rubython_ext.rb_types.RbObject'>")
  end

  it 'should unwrap RbObject wrappers which are returned from Python' do
    Canary = Class.new(Object)
    result = @PythonHelper.id(Canary.new)
    expect(result).to be_an_instance_of(Canary)
  end

  it 'should retain a reference to the context that it belongs to' do
    py_object = @context.py_eval('object()')
    expect(py_object.context).to eq(@context)
  end

  it 'should become invalid once the owning context is finalized' do
    py_object = @context.py_eval('object()')
    expect(py_object.is_valid?).to be_truthy
    PyContext.finalize!
    expect(py_object.is_valid?).to be_falsey
  end
end
