module Rubython
  module PyTypes
    class PyObject

      def method_missing(method, *args, **kwargs, &block)
        attr_key = method.to_s
        call_expected = !(args.empty? && kwargs.empty?) ||
              (attr_key[-1] == '!')
        attr_key = attr_key[0...-1] if attr_key[-1] == '!'

        begin
          py_attr = self.getattr(attr_key)
          if call_expected
            py_attr = py_attr.call(*args, **kwargs)
          end
          return py_attr
        rescue Exception => e
          super
        end
        super
      end
    end
  end
end
