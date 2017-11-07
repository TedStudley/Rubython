module Rubython
  module PyTypes
    class PyString
      def hash
        self.to_s.hash
      end

      def eql?(other)
        self.to_s == other.to_s
      end
    end
  end
end
