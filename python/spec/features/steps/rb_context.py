from behave import given, when, then
from hamcrest import *
from rubython import RbContext

@given('that the RbContext instance is null')
def step_impl(context):
    assert_that(RbContext.get_instance(), is_(none()))

@when('we create a new RbContext')
def step_impl(context):
    context.prev_rb_context = context.rb_context
    context.rb_context = RbContext()

@when('we tear down the RbContext')
def step_impl(context):
    RbContext.finalize()

@when('we populate the value of {key} with string "{value}"')
def step_impl(context, key, value):
    context.rb_context.rb_eval("{} = '{}'".format(key, value))

@when('we evaluate the value of string `{eval_str}`')
def step_impl(context, eval_str):
    context.rb_context.rb_eval(eval_str)

@then('the RbContext instance should be null')
def step_impl(context):
    assert_that(RbContext.get_instance(), is_(none()))

@then('the RbContext instance should be non-null')
def step_impl(context):
    assert_that(RbContext.get_instance(), is_(not_none()))

@then('that context should match a newly-created context wrapper')
def step_impl(context):
    rb_context = RbContext()
    assert_that(rb_context, is_(equal_to(context.rb_context)))

@then('that context should match the context instance')
def step_impl(context):
    assert_that(RbContext.get_instance(), is_(equal_to(context.rb_context)))

@then('the new RbContext should be distinct from the previous one')
def step_impl(context):
    assert_that(context.prev_rb_context, is_not(equal_to(context.rb_context)))

@then('the value of `{key}` should be string "{value}"')
def step_impl(context, key, value):
    test_value = context.rb_context.rb_eval("{}".format(key))
    assert_that(str(test_value), is_(equal_to(value)))

@then('attempting to eval `{eval_str}` should raise an exception')
def step_impl(context, eval_str):
    rb_context = context.rb_context
    assert_that(calling(rb_context.rb_eval).with_args(eval_str), raises(RuntimeError))

@then('attempting to eval `{eval_str}` should raise an exception matching "{exception_regex}"')
def step_impl(context, eval_str, exception_regex):
    assert_that(calling(context.rb_context.rb_eval).with_args(eval_str), raises(RuntimeError, exception_regex))

@then('we should not have crashed')
def step_impl(context):
    # If we're still here, we passed.
    assert_that(None, is_(none()))

@given('that the test environment is initialized with no active RbContext instance')
def step_impl(context):
    context.rb_context = None
    context.prev_rb_context = None
    RbContext.finalize()

