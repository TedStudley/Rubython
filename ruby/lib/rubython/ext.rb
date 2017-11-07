require 'rubython/common'

module Rubython
  module Ext
    require 'rubython/ext/code'

  end

  RUBYTHON_LOADED = true unless defined?(::Rubython::RUBYTHON_LOADED)
end
