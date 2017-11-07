from behave import *
import rubython
import imp

@given('that we have rubython installed')
def step_impl(context):
    pass

@when('we import rubython')
def step_impl(context):
    import rubython

@then('the rubython module should be in scope')
def step_impl(context):
    assert hasattr(rubython, '__version__')
    assert '0.1.0' == rubython.__version__

