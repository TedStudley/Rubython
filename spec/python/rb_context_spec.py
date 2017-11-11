from spec.python.environment import *

EXCEPTION_CASES = [
    ('foo',       'undefined local variable or method'),
    ('1 + "foo"', 'String can\'t be coerced into Fixnum'),
    ('1 / 0',     'divided by 0')
]

with describe('RbContext'):
    with after.each:
        RbContext.finalize()

    with it('should exist'):
        assert_that(RbContext, is_(not_none()))

    with it('should not be initialized before first use'):
        assert_that(RbContext.get_instance(), is_(none()))
        context = RbContext()
        assert_that(RbContext.get_instance(), is_(not_none()))

    with it('should be initializable'):
        rb_context = RbContext()
        assert_that(rb_context, is_(not_none()))

    with it('should share the same instance between all wrappers'):
        context_a = RbContext()
        context_b = RbContext()
        assert_that(context_a, is_(equal_to(context_b)))
        assert_that(context_b, is_(equal_to(RbContext.get_instance())))

    with it('should be able to be torn down'):
        context = RbContext()
        assert_that(RbContext.get_instance(), is_(not_none()))
        RbContext.finalize()
        assert_that(RbContext.get_instance(), is_(none()))

    with it('should raise an exception when attempting to reference a finalized context'):
        context = RbContext()
        RbContext.finalize()
        assert_that(calling(context.global_variables), raises(RuntimeError))

    with it('should allow creating a new context after finalizing a previous one'):
        context_a = RbContext()
        RbContext.finalize()
        context_b = RbContext()
        assert_that(context_a, is_not(equal_to(context_b)))
        assert_that(context_b, is_(equal_to(RbContext.get_instance())))


    with it('should be able to influence values in Ruby'):
        context = RbContext()
        context.rb_eval("foo = 'bar'")
        # We should have a new local variable
        assert_that(str(context.rb_eval('foo')), is_(equal_to('bar')))

    with it('should be able to list the local variables in a context'):
        context = RbContext()
        assert_that(str(context.local_variables()), is_(equal_to('[]')))
        context.rb_eval('foo = "bar"')
        assert_that(str(context.local_variables()), is_(equal_to('[:foo]')))

    with it('should be able to list the global variables in a context'):
        context = RbContext()
        assert_that(str(context.global_variables()), starts_with("[:$;, :$-F, :$@, :$!, :$SAFE"))
        context.rb_eval("$FOO = 'bar'")
        assert_that(str(context.global_variables()), matches_regexp(":\$FOO"))

    with it('should be able to list the constants in a context'):
        context = RbContext()
        assert_that(str(context.constants()), starts_with("[:Object, :Module, :Class, :BasicObject, :Kernel"))
        context.rb_eval("FOO = 'bar'")
        assert_that(str(context.constants()), matches_regexp(":FOO"))

    with it('should properly refresh the context state between context instances'):
        context_a = RbContext()
        context_a.rb_eval("foo = 'bar'")
        assert_that(str(context_a.local_variables()), is_(equal_to('[:foo]')))
        RbContext.finalize()
        context_b = RbContext()
        assert_that(str(context_b.local_variables()), is_(equal_to('[]')))
        assert_that(calling(context_b.rb_eval).with_args("foo"), raises(RuntimeError))

    with it('should be able to manipulate values within Ruby'):
        context = RbContext()
        context.rb_eval('baz = "foo"')
        context.rb_eval('qux = "bar"')
        assert_that(str(context.rb_eval("baz + qux")), is_(equal_to('foobar')))

    with it('should raise exceptions with messages forwarded from Ruby'):
        context = RbContext()
        for (eval_str, expected_message) in EXCEPTION_CASES:
            faulty_call = calling(context.rb_eval).with_args(eval_str)
            assert_that(faulty_call, raises(RuntimeError, expected_message))

    with it('should be able to modify methods from the Ruby side'):
        context = RbContext()
        context.rb_eval("foobar = 'foo'")
        context.rb_eval("foobar += 'bar'")
        assert_that(str(context.rb_eval('foobar')), is_(equal_to('foobar')))
