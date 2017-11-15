module Rubython
  module PyTypes
    class PyObject

      private
      def method_missing(method, *args, **kwargs, &block)
        attr_key = method.to_s
        call_expected = !(args.empty? && kwargs.empty?) ||
              (attr_key[-1] == '!')
        attr_key = attr_key[0...-1] if attr_key[-1] == '!'

        super unless self.hasattr(attr_key)
        py_attr = self.getattr(attr_key)
        py_attr = py_attr.call(*args, **kwargs) if call_expected

        return py_attr
      end
    end
  end
end
