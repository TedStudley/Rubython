from os import path

from hamcrest import *

from rubython import *
from rubython.rubython_ext.rb_types import *

RUBY_HELPER_PATH = path.realpath(path.join(path.dirname(__file__), 'helpers', 'ruby_helper.rb'))
