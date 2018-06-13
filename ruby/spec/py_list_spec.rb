require_relative './spec_helper'

describe PyList do
  before(:each) do
    @context = PyContext.new(PYTHON_HELPER_PATH)
    @PythonHelper = @context.py_eval("PythonHelper")
  end

  after(:each) do
    PyContext.finalize!
  end

  it 'exists' do
    expect(PyList).to be
  end

  it 'should be comparable to a raw Ruby Array' do
    py_list = @context.py_eval('[]')
    expect(py_list).to eq([])
    py_list = @context.py_eval('[1,2,3]')
    expect(py_list).to eq([1,2,3])
  end

  it 'should be possible to exfiltrate a PyList reference from Python via the PyContext' do
    py_list = @context.py_eval('[]')
    expect(py_list).to be_an(PyList)
    expect(py_list).to eq([])
  end

  it 'should be possible to dereference list elements' do
    py_list = @context.py_eval('[1, 2, 3]')
    expect(py_list[1]).to eq(2)
  end

  it 'should be possible to assign list elements' do
    py_list = @context.py_eval('[1, 2, 3]')
    py_list[1] = 4
    expect(py_list).to eq([1, 4, 3])
  end

  it 'should be possible to assign Ruby objects to list elements' do
    py_list = @context.py_eval('[None]')
    canary = Object.new
    py_list[0] = canary
    expect(py_list[0]).to eq(canary)
  end

  describe 'proper Python list behavior' do
    it 'should throw on out-of-bounds indexing' do
      py_list = @context.py_eval('[]')
      expect { py_list[1] }.to raise_error(RuntimeError, /list index out of range/)
    end

    it 'should throw on out-of-bounds assignment' do
      py_list = @context.py_eval('[]')
      expect { py_list[1] = nil }.to raise_error(RuntimeError, /list assignment index out of range/)
    end

  end

  describe 'Python list interface' do
    it 'should behave properly for `append`' do
      py_list = @context.py_eval('[]')
      py_list.append(42)
      expect(py_list).to eq([42])
    end

    it 'should behave properly for `extend`' do
      py_list = @context.py_eval('[1,2,3]')
      py_list.extend([4,5,6])
      expect(py_list).to eq([1,2,3,4,5,6])
    end

    it 'should behave properly for `insert`' do
      py_list = @context.py_eval('[42]')
      py_list.insert(0, 7)
      expect(py_list).to eq([7, 42])
      py_list.insert(2, 11)
      expect(py_list).to eq([7, 42, 11])
    end

    it 'should behave properly for `remove`' do
      py_list = @context.py_eval('[1,2,3]')
      py_list.remove(2)
      expect(py_list).to eq([1,3])
      expect {py_list.remove(42)}.to raise_error(RuntimeError, /x not in list/)
    end

    it 'should behave properly for `pop`' do
      py_list = @context.py_eval('[1, 2, 3, 4, 5]')
      expect(py_list.pop()).to eq(5)
      expect(py_list).to eq([1,2,3,4])
      expect(py_list.pop(0)).to eq(1)
      expect(py_list).to eq([2,3,4])
    end

    it 'should behave properly for `index`' do
      py_list = @context.py_eval('[1,2,3,2,1]')
      expect(py_list.index(3)).to eq(2)
      expect(py_list.index(1)).to eq(0)
      expect { py_list.index(7) }.to raise_error(RuntimeError, /7 is not in list/)
    end

    it 'should behave properly for `count`' do
      py_list = @context.py_eval('[1, 2, 2, 3, 3, 3]')
      expect(py_list.count(1)).to eq(1)
      expect(py_list.count(2)).to eq(2)
      expect(py_list.count(3)).to eq(3)
      expect(py_list.count(4)).to eq(0)
    end

    it 'should behave properly for `sort`' do
      py_list = @context.py_eval('[-11, -1, 4, 32, 7, 42]')

      # Basic sort should work as expected
      py_list.sort()
      expect(py_list).to eq([-11, -1, 4, 7, 32, 42])

      # Passing a comparison function should work as expected
      py_list.sort(proc { |a, b| (a % 3) <=> (b % 3) })
      expect(py_list).to eq([42, -11, 4, 7, -1, 32])

      # Passing a key function should work as expected
      key = @context.int.getattr('__abs__')
      py_list.sort(nil, key)
      expect(py_list).to eq([-1, 4, 7, -11, 32, 42])

      # reverse sort should work as expected
      py_list.sort(nil, nil, true)
      expect(py_list).to eq([42, 32, 7, 4, -1, -11])

      # Some god-awful combination of all three should work as expected
      py_list.sort(proc { |a, b| (a % 3) <=> (b % 3) }, key, true)
      expect(py_list).to eq([32, -11, 7, 4, -1, 42])
    end

    it 'should behave properly for `reverse`' do
      py_list = @context.py_eval('[1, 2, 3]')
      py_list.reverse()
      expect(py_list).to eq([3, 2, 1])
    end

    it 'should match the standard Python list example behavior' do
      # https://docs.python.org/2/tutorial/datastructures.html?highlight=list
      a = @context.py_eval('[66.25, 333, 333, 1, 1234.5]')

      expect(a.count(333)).to eq(2)
      expect(a.count(66.25)).to eq(1)
      expect(a.count('x')).to eq(0)

      a.insert(2, -1)
      a.append(333)
      expect(a).to eq([66.25, 333, -1, 333, 1, 1234.5, 333])
      expect(a.index(333)).to eq(1)

      a.remove(333)
      expect(a).to eq([66.25, -1, 333, 1, 1234.5, 333])

      a.reverse()
      expect(a).to eq([333, 1234.5, 1, 333, -1, 66.25])

      a.sort()
      expect(a).to eq([-1, 1, 66.25, 333, 333, 1234.5])
      expect(a.pop()).to eq(1234.5)
      expect(a).to eq([-1, 1, 66.25, 333, 333])
    end
  end
end
