Feature: RbContext Ruby Functionality
  The RbContext object should provide a bridge to a Ruby context.

  Background: The RbContext instance should be cleared before each test
    Given that the RbContext instance is torn down

  Scenario: RbContext wrappers are Initializable
    Given that RbContext instance is null
    When we create a new RbContext
    Then the RbContext instance should be non-null

  Scenario: RbContext wrappers share the same instance
    Given that RbContext instance is null
    When we create a new RbContext
    Then that context should match a newly-created context wrapper
     and that context should match the context instance
