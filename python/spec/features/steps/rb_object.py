from behave import given, when, then
from hamcrest import *
from rubython import RbContext
from rubython.rubython_ext.rb_types import RbObject

@given('that we create a new RbContext')
def step_impl(context):
    context.rb_context = RbContext()

@when('we evaluate `{eval_str}` within the RbContext')
def step_impl(context, eval_str):
    context.rb_context.rb_eval(eval_str)

@when('we evaluate `{eval_str}` within the RbContext and save the result')
def step_impl(context, eval_str):
    context.result = context.rb_context.rb_eval(eval_str)

@when('we evaluate `{eval_str} within the RbContext and save the result as "{result_key}"')
def step_impl(context, eval_str, result_key):
    if not hasattr(context, 'results'):
        context.results = {}
    context.results[result_key] = context.rb_context.rb_eval(eval_str)

@when('we save the result of calling `{eval_str}` on the saved object as "{result_key}"')
def step_impl(context, eval_str, result_key):
    if not hasattr(context, 'results'):
        context.results = {}
    context.results[result_key] = getattr(context.result, eval_str)()

@then('the result should be a valid RbObject')
def step_impl(context):
    assert_that(context.result, is_(not_none()))
    assert_that(context.result, is_(instance_of(RbObject)))

@then('the result of calling `{eval_str}` on the saved result "{result_key}" should be "{result_str}"')
def step_impl(context, eval_str, result_key, result_str):
    result = getattr(context.results[result_key], eval_str)
    assert_that(str(result()), is_(equal_to(result_str)))

@then('the result of calling `{eval_str}` on the saved object should be "{result_str}"')
def step_impl(context, eval_str, result_str):
    result = getattr(context.result, eval_str)
    assert_that(str(result()), is_(equal_to(result_str)))

@then('the result of calling `{eval_str}` on that object should match "{result_regex}"')
def step_impl(context, eval_str, result_regex):
    result = getattr(context.result, eval_str)
    assert_that(str(result()), matches_regexp(result_regex))

@then('the string value of saved result "{result_key}" should be "{result_str}"')
def step_impl(context, result_key, result_str):
    result = context.results[result_key]
    assert_that(str(result), is_(equal_to(result_str)))

@then('the string values of saved results "{key_a}" and "{key_b}" should be distinct')
def step_impl(context, key_a, key_b):
    result_a = context.results[key_a]
    result_b = context.results[key_b]
    assert_that(str(result_a), is_not(equal_to(str(result_b))))
