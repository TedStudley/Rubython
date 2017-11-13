from spec.environment import *
from rubython.rubython_ext.rb_types import RbObject

with describe('RbObject'):
    with before.each:
        self.context = RbContext('spec/python/ruby_helper.rb')
        self.rb_helper = self.context.rb_eval("RubyHelper")

    with after.each:
        RbContext.finalize()

    with it('should be possible to exfiltrate an RbObject reference from Ruby via the RbContext'):
        rb_object = self.context.rb_eval('Object.new')
        assert_that(rb_object, is_(instance_of(RbObject)))
        assert_that(rb_object, is_(not_none()))

    with it('should be possible to call methods from Ruby with no arguments on an RbObject'):
        rb_object = self.context.rb_eval('Object.new')
        result = rb_object.to_s()
        assert_that(str(result), matches_regexp('^#<Object'))

    with it('should be possible to call methods from Ruby with arguments on an RbObject'):
        rb_object = self.context.rb_eval('[]')
        rb_object.unshift(42)
        assert_that(str(rb_object), is_(equal_to('[42]')))

    with it('should be possible to affect the Ruby context with Ruby methods called on an RbObject'):
        rb_object = self.context.rb_eval('foo = []')
        rb_object.unshift(42)
        result = self.context.rb_eval('foo')
        assert_that(str(result), is_(equal_to('[42]')))

    with it('should provide a proper reference to an object in Ruby'):
        rb_object = self.context.rb_eval('foo = []')
        assert_that(str(rb_object), is_(equal_to('[]')))
        self.context.rb_eval('foo << 42')
        assert_that(str(rb_object), is_(equal_to('[42]')))

    with it('should pass Python objects to Ruby within PyObject wrappers'):
        test_object = object()
        ruby_klass = self.rb_helper.get_rubyklass(test_object)
        assert_that(str(ruby_klass), is_(equal_to('Rubython::PyTypes::PyObject')))

    with it('should unwrap PyObject wrappers which are returned from Ruby'):
        class canary:
            pass
        result = self.rb_helper.id(canary())
        assert_that(result, is_(instance_of(canary)))
