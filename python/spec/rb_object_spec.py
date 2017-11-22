from spec.environment import *

with describe(RbObject):
    with before.each:
        RbContext.finalize() # TODO: Why does this need to be here?
        self.context = RbContext(RUBY_HELPER_PATH)
        self.RubyHelper = self.context.rb_eval("RubyHelper")

    with after.each:
        RbContext.finalize()

    with it('is possible to exfiltrate an RbObject reference from Ruby via the RbContext'):
        rb_object = self.context.rb_eval('Object.new')
        assert_that(rb_object, is_(instance_of(RbObject)))

    with it('is possible to call methods from Ruby with no arguments on an RbObject'):
        rb_object = self.context.rb_eval('"foo"')
        assert_that(rb_object.upcase(), is_(equal_to('FOO')))

    with it('is possible to call methods from Ruby with arguments on an RbObject'):
        rb_object = self.context.rb_eval('[]')
        rb_object.unshift(42)
        assert_that(rb_object, is_(equal_to([42])))

    with it('is possible to affect the Ruby context with Ruby methods called on an RbObject'):
        rb_object = self.context.rb_eval('foo = []')
        rb_object.unshift(42)
        result = self.context.rb_eval('foo')
        assert_that(result, is_(equal_to([42])))

    with it('provides a proper reference to an object in Ruby'):
        rb_object = self.context.rb_eval('foo = []')
        assert_that(rb_object, is_(equal_to([])))
        self.context.rb_eval('foo.unshift(42)')
        assert_that(rb_object, is_(equal_to([42])))

    with it('passes Python objects to Ruby within PyObject wrappers'):
        test_object = object()
        ruby_klass = self.RubyHelper.get_rubyklass(test_object)
        assert_that(str(ruby_klass), is_(equal_to('Rubython::PyTypes::PyObject')))

    with it('should unwrap PyObject wrappers which are returned from Ruby'):
        class canary:
            pass
        result = self.RubyHelper.id(canary())
        assert_that(result, is_(instance_of(canary)))

    with it('should retain a reference to the context that it belongs to'):
        rb_object = self.context.rb_eval("Object.new")
        assert_that(rb_object.__context__, is_(equal_to(self.context)))

    with it('becomes invalid once the owning context is finalized'):
        rb_object = self.context.rb_eval("Object.new")
        assert_that(rb_object.is_valid(), is_(equal_to(True)))
        RbContext.finalize()
        assert_that(rb_object.is_valid(), is_(equal_to(False)))

