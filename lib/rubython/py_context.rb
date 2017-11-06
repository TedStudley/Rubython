module Rubython
  class PyContext
    def method_missing(method, *args, **kwargs)
      attr_key = method.to_s
      call_expected = !(args.empty? && kwargs.empty?) ||
          (attr_key[-1] == '!')
      attr_key = attr_key[0...-1] if attr_key[-1] == '!'

      begin
        py_attr = self.py_eval(attr_key)
        py_attr = py_attr.call(*args, **kwargs) if call_expected

        return py_attr
      rescue Exception => e
        raise
      end
      super
    end
  end
end
