require_relative './spec_helper'

describe PyString do
  before(:each) do
    @context = PyContext.new(PYTHON_HELPER_PATH)
    @PythonHelper = @context.py_eval("PythonHelper")
    @context.py_exec("import sys")
  end

  after(:each) do
    PyContext.finalize!
  end

  it 'exists' do
    expect(PyString).to be
  end

  it 'should be comparable to a raw Ruby String' do
    py_string = @context.py_eval('""')
    expect(py_string).to eq('')
    py_string = @context.py_eval('"foo"')
    expect(py_string).to eq('foo')
  end

  it 'should be possible to exfiltrate a PyString reference from Python via the PyContext' do
    py_string = @context.py_eval('"foo"')
    expect(py_string).to be_a(PyString)
    expect(py_string).to eq('foo')
  end

  describe 'Python string interface' do
    def string(string)
      @context.py_eval(string.inspect)
    end

    it 'should behave properly for `count`' do
      str1 = string('aaa')
      expect(str1.count('a')).to eq(3)
      expect(str1.count('b')).to eq(0)
      expect(str1.count('a')).to eq(3)
      expect(str1.count('b')).to eq(0)
      expect(str1.count('a', 1)).to eq(2)
      expect(str1.count('a', 10)).to eq(0)
      expect(str1.count('a', -1)).to eq(1)
      expect(str1.count('a', -10)).to eq(3)
      expect(str1.count('a', 0, 1)).to eq(1)
      expect(str1.count('a', 0, 10)).to eq(3)
      expect(str1.count('a', 0, -1)).to eq(2)
      expect(str1.count('a', 0, -10)).to eq(0)
      expect(str1.count('', 1)).to eq(3)
      expect(str1.count('', 3)).to eq(1)
      expect(str1.count('', 10)).to eq(0)
      expect(str1.count('', -1)).to eq(2)
      expect(str1.count('', -10)).to eq(4)

      str2 = string('')
      expect(str2.count('')).to eq(1)
      expect(str2.count('', 1, 1)).to eq(0)
      expect(str2.count('', @context.sys.maxsize, 0)).to eq(0)

      expect(str2.count('xx')).to eq(0)
      expect(str2.count('xx', 1, 1)).to eq(0)
      expect(str2.count('xx', @context.sys.maxsize, 0)).to eq(0)

      expect { string('hello').count }.to raise_error(RuntimeError)
    end

    it 'should behave properly for `find`' do
      str1 = string('abcdefghiabc')
      expect(str1.find('abc')).to eq(0)
      expect(str1.find('abc', 1)).to eq(9)
      expect(str1.find('def', 4)).to eq(-1)

      str2 = string('abc')
      expect(str2.find('', 0)).to eq(0)
      expect(str2.find('', 3)).to eq(3)
      expect(str2.find('', 4)).to eq(-1)

      str3 = string('rrarrrrrrrrra')
      expect(str3.find('a')).to eq(2)
      expect(str3.find('a', 4)).to eq(12)
      expect(str3.find('a', 4, 6)).to eq(-1)
      expect(str3.find('a', 4, nil)).to eq(12)
      expect(str3.find('a', nil, 6)).to eq(2)

      expect { string('hello').find }.to raise_error(RuntimeError)

      str3 = string('')
      expect(str3.find('')).to eq(0)
      expect(str3.find('', 1, 1)).to eq(-1)
      expect(str3.find('', @context.sys.maxsize, 0)).to eq(-1)

      expect(string('ab').find('xxx', @context.sys.maxsize + 1, 0)).to eq(-1)
    end

    it 'should behave properly for `rfind`' do
      str1 = string('abcdefghiabc')
      expect(str1.rfind('abc')).to eq(9)
      expect(str1.rfind('')).to eq(12)
      expect(str1.rfind('abcd')).to eq(0)
      expect(str1.rfind('abcz')).to eq(-1)

      str2 = string('abc')
      expect(str2.rfind('', 0)).to eq(3)
      expect(str2.rfind('', 3)).to eq(3)
      expect(str2.rfind('', 4)).to eq(-1)

      str3 = string('rrarrrrrrrrra')
      expect(str3.rfind('a')).to eq(12)
      expect(str3.rfind('a', 4)).to eq(12)
      expect(str3.rfind('a', 4, 6)).to eq(-1)
      expect(str3.rfind('a', 4, nil)).to eq(12)
      expect(str3.rfind('a', nil, 6)).to eq(2)

      expect { string('hello').find }.to raise_error(RuntimeError)

      expect(string('ab').rfind('xxx', @context.sys.maxsize + 1, 0)).to eq(-1)
      expect(string('<......\u043c...').rfind('<')).to eq(0)
    end

    it 'should behave properly for `index`' do
      str1 = string('abcdefghiabc')
      expect(str1.index('')).to eq(0)
      expect(str1.index('def')).to eq(3)
      expect(str1.index('abc')).to eq(0)
      expect(str1.index('abc', 1)).to eq(9)

      expect { str1.index('hib') }.to raise_error(RuntimeError)
      expect { string('abcdefghiab').index('abc', 1) }.to raise_error(RuntimeError)
      expect { string('abcdefghi').index('ghi', 8) }.to raise_error(RuntimeError)
      expect { string('abcdefghi').index('ghi', -1) }.to raise_error(RuntimeError)

      str2 = string('rrarrrrrrrrra')
      expect(str2.index('a')).to eq(2)
      expect(str2.index('a', 4)).to eq(12)
      expect { str2.index('a', 4, 6) }.to raise_error(RuntimeError)
      expect(str2.index('a', 4, nil)).to eq(12)
      expect(str2.index('a', nil, 6)).to eq(2)

      expect { string('hello').index }.to raise_error(RuntimeError)
    end

    it 'should behave properly for `rindex`' do
      str1 = string('abcdefghiabc')
      expect(str1.rindex('')).to eq(12)
      expect(str1.rindex('def')).to eq(3)
      expect(str1.rindex('abc')).to eq(9)
      expect(str1.rindex('abc', 0, -1)).to eq(0)

      expect { str1.rindex('hib') }.to raise_error(RuntimeError)
      expect { string('defghiabc').rindex('def', 1) }.to raise_error(RuntimeError)
      expect { string('defghiabc').rindex('abc', 0, -1) }.to raise_error(RuntimeError)
      expect { string('abcdefghi').rindex('ghi', 0, 8) }.to raise_error(RuntimeError)
      expect { string('abcdefghi').rindex('ghi', 0, -1) }.to raise_error(RuntimeError)

      str2 = string('rrarrrrrrrrra')
      expect(str2.rindex('a')).to eq(12)
      expect(str2.rindex('a', 4)).to eq(12)
      expect { str2.rindex('a', 4, 6) }.to raise_error(RuntimeError)
      expect(str2.rindex('a', 4, nil)).to eq(12)
      expect(str2.rindex('a', nil, 6)).to eq(2)

      expect { string('hello').rindex }.to raise_error(RuntimeError)
    end

    it 'should behave properly for `lower`' do
      expect(string('HeLLo').lower).to eq('hello')
      expect(string('hello').lower).to eq('hello')
      expect { string('hello').lower(42) }.to raise_error(RuntimeError)
    end

    it 'should behave properly for `upper`' do
      expect(string('HeLLo').upper).to eq('HELLO')
      expect(string('HELLO').upper).to eq('HELLO')
      expect { string('HELLO').upper(42) }.to raise_error(RuntimeError)
    end

    it 'should behave properly for `expandtabs`' do
      str1 = string("abc\rab\tdef\ng\thi")
      expect(str1.expandtabs).to eq("abc\rab      def\ng       hi")
      expect(str1.expandtabs(8)).to eq("abc\rab      def\ng       hi")
      expect(str1.expandtabs(4)).to eq("abc\rab  def\ng   hi")

      expect { string('hello').expandtabs(42, 42) }.to raise_error(RuntimeError)
    end

    it 'should behave properly for `split`' do
      str1 = string('a|b|c|d')
      expect(str1.split('|')).to eq(['a', 'b', 'c', 'd'])
      expect(str1.split('|', 0)).to eq(['a|b|c|d'])
      expect(str1.split('|', 1)).to eq(['a', 'b|c|d'])
      expect(str1.split('|', 2)).to eq(['a', 'b', 'c|d'])
      expect(str1.split('|', 3)).to eq(['a', 'b', 'c', 'd'])
      expect(str1.split('|', 4)).to eq(['a', 'b', 'c', 'd'])
      expect(str1.split('|', @context.sys.maxsize - 2)).to eq(['a', 'b', 'c', 'd'])
      expect(str1.split('|', 0)).to eq(['a|b|c|d'])

      expect(string('a||b||c||d').split('|', 2)).to eq(['a', '', 'b||c||d'])
      expect(string('abcd').split('|')).to eq(['abcd'])
      expect(string('').split('|')).to eq([''])
      expect(string('endcase |').split('|')).to eq(['endcase ', ''])
      expect(string('| startcase').split('|')).to eq(['', ' startcase'])
      expect(string('|bothcase|').split('|')).to eq(['', 'bothcase', ''])
      # TODO: Fix this
      # expect(string("a\x00\x00b\x00c\x00d").split("\x00", 2)).to eq(['a', '', 'b\x00c\x00d'])

      expect { string('hello').split(42, 42, 42) }.to raise_error(RuntimeError)

      expect { string('hello').split('') }.to raise_error(RuntimeError)
      expect { string('hello').split('', 0) }.to raise_error(RuntimeError)
    end

    it 'should behave properly for `rsplit`' do
    end

    it 'should behave properly for `replace`' do
      str1 = string('')
      expect(str1.replace('', '')).to eq('')
      expect(str1.replace('', 'A')).to eq('A')
      expect(str1.replace('A', '')).to eq('')
      expect(str1.replace('A', 'A')).to eq('')
      expect(str1.replace('', '', 100)).to eq('')
      expect(str1.replace('', '', @context.sys.maxsize)).to eq('')

      str2 = string('A')
      expect(str2.replace('', '')).to eq('A')
      expect(str2.replace('', '*')).to eq('*A*')
      expect(str2.replace('', '*1')).to eq('*1A*1')
      expect(str2.replace('', '*-#')).to eq('*-#A*-#')
      str3 = string('AA')
      expect(str3.replace('', '*-')).to eq('*-A*-A*-')
      expect(str3.replace('', '*-', -1)).to eq('*-A*-A*-')
      expect(str3.replace('', '*-', @context.sys.maxsize)).to eq('*-A*-A*-')
      expect(str3.replace('', '*-', 4)).to eq('*-A*-A*-')
      expect(str3.replace('', '*-', 3)).to eq('*-A*-A*-')
      expect(str3.replace('', '*-', 2)).to eq('*-A*-A')
      expect(str3.replace('', '*-', 1)).to eq('*-AA')
      expect(str3.replace('', '*-', 0)).to eq('AA')

      expect(str2.replace('A', '')).to eq('')
      str4 = string('AAA')
      expect(str4.replace('A', '')).to eq('')
      expect(str4.replace('A', '', -1)).to eq('')
      expect(str4.replace('A', '', @context.sys.maxsize)).to eq('')
      expect(str4.replace('A', '', 4)).to eq('')
      expect(str4.replace('A', '', 3)).to eq('')
      expect(str4.replace('A', '', 2)).to eq('A')
      expect(str4.replace('A', '', 1)).to eq('AA')
      expect(str4.replace('A', '', 0)).to eq('AAA')
      expect(string('AAAAAAAAAA').replace('A', '')).to eq('')
      str5 = string('ABACADA')
      expect(str5.replace('A', '')).to eq('BCD')
      expect(str5.replace('A', '', -1)).to eq('BCD')
      expect(str5.replace('A', '', @context.sys.maxsize)).to eq('BCD')
      expect(str5.replace('A', '', 4)).to eq('BCD')
      expect(str5.replace('A', '', 3)).to eq('BCDA')
      expect(str5.replace('A', '', 2)).to eq('BCADA')
      expect(str5.replace('A', '', 1)).to eq('BACADA')
      expect(str5.replace('A', '', 0)).to eq('ABACADA')

      str6 = string('hello')
      expect { str6.replace }.to raise_error(RuntimeError)
      expect { str6.replace(42) }.to raise_error(RuntimeError)
      expect { str6.replace(42, 'h') }.to raise_error(RuntimeError)
      expect { str6.replace('h', 42) }.to raise_error(RuntimeError)
    end

    it 'should behave properly for `strip`' do
      str1 = string('xyzzyhelloxyzzy')
      arg1 = string('xyz')
      expect(str1.strip(arg1)).to eq('hello')
      expect(str1.lstrip(arg1)).to eq('helloxyzzy')
      expect(str1.rstrip(arg1)).to eq('xyzzyhello')
      expect(str1.strip(arg1)).to eq('hello')

      str2 = string('mississippi')
      arg2 = string('i')
      expect(str2.strip(str2)).to eq('')
      expect(str2.strip(arg2)).to eq('mississipp')

      str3 = string('hello')
      expect { str3.strip(42, 42) }.to raise_error(RuntimeError)
      expect { str3.lstrip(42, 42) }.to raise_error(RuntimeError)
      expect { str3.rstrip(42, 42) }.to raise_error(RuntimeError)
    end

    it 'should behave properly for `capitalize`' do
    end

    it 'should behave properly for `casefold`' do
    end

    it 'should behave properly for `center`' do
    end

    it 'should behave properly for `encode`' do
    end

    it 'should behave properly for `endswith`' do
    end

    it 'should behave properly for `format`' do
    end

    it 'should behave properly for `format_map`' do
    end

    it 'should behave properly for `isalnum`' do
    end

    it 'should behave properly for `isalpha`' do
    end

    it 'should behave properly for `isdecimal`' do
    end

    it 'should behave properly for `isdigit`' do
    end

    it 'should behave properly for `isidentifier`' do
    end

    it 'should behave properly for `islower`' do
    end

    it 'should behave properly for `isnumeric`' do
    end

    it 'should behave properly for `isprintable`' do
    end

    it 'should behave properly for `isspace`' do
    end

    it 'should behave properly for `istitle`' do
    end

    it 'should behave properly for `isupper`' do
    end

    it 'should behave properly for `join`' do
    end

    it 'should behave properly for `ljust`' do
    end

    it 'should behave properly for `lstrip`' do
    end

    it 'should behave properly for `maketrans`' do
    end

    it 'should behave properly for `partition`' do
    end

    it 'should behave properly for `rjust`' do
    end

    it 'should behave properly for `rpartition`' do
    end

    it 'should behave properly for `rstrip`' do
    end

    it 'should behave properly for `splitlines`' do
    end

    it 'should behave properly for `startswith`' do
    end

    it 'should behave properly for `swapcase`' do
    end

    it 'should behave properly for `title`' do
    end

    it 'should behave properly for `translate`' do
    end

    it 'should behave properly for `zfill`' do
    end
  end
end
