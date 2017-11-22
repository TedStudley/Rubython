from spec.environment import *

EXCEPTION_CASES = [
    ('foo',       'undefined local variable or method'),
    ('1 + "foo"', 'String can\'t be coerced into Fixnum'),
    ('1 / 0',     'divided by 0')
]

with describe('RbContext'):
    with after.each:
        RbContext.finalize()

    with it('exists'):
        assert_that(RbContext, is_(not_none()))

    with it('is not initialized before first use'):
        assert_that(RbContext.get_instance(), is_(none()))
        context = RbContext()
        assert_that(RbContext.get_instance(), is_(not_none()))

    with it('shares the same instance between all wrappers'):
        context_a = RbContext()
        context_b = RbContext()
        assert_that(context_a, is_(equal_to(context_b)))
        assert_that(context_b, is_(equal_to(RbContext.get_instance())))

    with it('is able to be torn down'):
        context = RbContext()
        assert_that(RbContext.get_instance(), is_(not_none()))
        RbContext.finalize()
        assert_that(RbContext.get_instance(), is_(none()))

    with it('raises an exception when attempting to reference a finalized context'):
        context = RbContext()
        RbContext.finalize()
        assert_that(calling(context.global_variables), raises(RuntimeError))

    with it('allows creating a new context after finalizing a previous one'):
        context_a = RbContext()
        RbContext.finalize()
        context_b = RbContext()
        assert_that(context_a, is_not(equal_to(context_b)))
        assert_that(context_b, is_(equal_to(RbContext.get_instance())))

    with it('is able to influence values in Ruby'):
        context = RbContext()
        context.rb_eval("foo = 'bar'")
        # We should have a new local variable
        assert_that(context.rb_eval('foo'), is_(equal_to('bar')))

    with it('is able to list the local variables in a context'):
        context = RbContext()
        assert_that(context.local_variables()), is_(equal_to([]))
        context.rb_eval('foo = "bar"')
        assert_that(str(context.local_variables()), is_(equal_to('[:foo]')))

    with it('is able to list the global variables in a context'):
        context = RbContext()
        assert_that(str(context.global_variables()), starts_with("[:$;, :$-F, :$@, :$!, :$SAFE"))
        context.rb_eval("$FOO = 'bar'")
        assert_that(str(context.global_variables()), matches_regexp(":\$FOO"))

    with it('is able to list the constants in a context'):
        context = RbContext()
        assert_that(str(context.constants()), starts_with("[:Object, :Module, :Class, :BasicObject, :Kernel"))
        context.rb_eval("FOO = 'bar'")
        assert_that(str(context.constants()), matches_regexp(":FOO"))

    with it('completely refreshes the context state between context instances'):
        context_a = RbContext()
        context_a.rb_eval("foo = 'bar'")
        assert_that(str(context_a.local_variables()), is_(equal_to('[:foo]')))
        RbContext.finalize()
        context_b = RbContext()
        assert_that(str(context_b.local_variables()), is_(equal_to('[]')))
        assert_that(calling(context_b.rb_eval).with_args("foo"), raises(RuntimeError))

    with it('is able to manipulate values within Ruby'):
        context = RbContext()
        context.rb_eval('baz = "foo"')
        context.rb_eval('qux = "bar"')
        assert_that(context.rb_eval("baz + qux"), is_(equal_to('foobar')))

    with it('raises exceptions with messages forwarded from Ruby'):
        context = RbContext()
        for (eval_str, expected_message) in EXCEPTION_CASES:
            faulty_call = calling(context.rb_eval).with_args(eval_str)
            assert_that(faulty_call, raises(RubythonError, expected_message))

    with it('is able to modify methods from the Ruby side'):
        context = RbContext()
        context.rb_eval("foobar = 'foo'")
        context.rb_eval("foobar += 'bar'")
        assert_that(context.rb_eval('foobar'), is_(equal_to('foobar')))

    with it('is able to access local variables by name'):
        context = RbContext()
        context.rb_eval("foo = 'bar'")
        assert_that(context.foo, is_(equal_to('bar')))

    with it('is able to call methods from the Ruby context'):
        context = RbContext()
        context.rb_eval("""
        def foo(x)
          return 2*x
        end
        """)
        assert_that(context.foo(21), is_(equal_to(42)))
