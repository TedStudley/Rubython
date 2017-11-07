from behave import *
from rubython import (RbContext)

@given('that the RbContext instance is torn down')
def step_impl(context):
    RbContext.tear_down()

@given('that RbContext instance is null')
def step_impl(context):
    assert RbContext.instance == None

@when('we create a new RbContext')
def step_impl(context):
    context.rb_context = RbContext()

@then('the RbContext instance should be non-null')
def step_impl(context):
    assert RbContext.instance != None

@then('that context should match a newly-created context wrapper')
def step_impl(context):
    rb_context = RbContext()
    assert rb_context == context.rb_context

@then('that context should match the context instance')
def step_impl(context):
    assert RbContext.instance == rb_context
