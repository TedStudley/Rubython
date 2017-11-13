Feature: RbContext Ruby Functionality
  The RbContext object should provide a bridge to a Ruby context.

  Background: The RbContext instance should be cleared before each test
    Given that the test environment is initialized with no active RbContext instance

  Scenario: RbContext wrappers are Initializable
    When we create a new RbContext
    Then the RbContext instance should be non-null

  Scenario: RbContext wrappers share the same instance
     When we create a new RbContext
     Then that context should match a newly-created context wrapper
      And that context should match the context instance

  Scenario: RbContext wrappers should be able to be torn down
     When we create a new RbContext
      And we tear down the RbContext
     Then the RbContext instance should be null

  Scenario: RbContext wrappers should be able to be re-created after being torn down
     When we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
     Then the RbContext instance should be non-null

  Scenario: RbContext wrappers should be able to be torn down and re-initialized repeatedly
     When we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
      And we tear down the RbContext
      And we create a new RbContext
      And we tear down the RbContext
     Then we should not have crashed

  Scenario: RbContext wrappers should be able to populate values in Ruby
     When we create a new RbContext
      And we populate the value of foo with string "bar"
     Then the value of `foo` should be string "bar"

  Scenario: RbContext wrappers should be able to manipulate values in Ruby
     When we create a new RbContext
      And we populate the value of baz with string "foo"
      And we populate the value of qux with string "bar"
     Then the value of `baz+qux` should be string "foobar"

  Scenario Outline: Exceptions
     When we create a new RbContext
     Then attempting to eval `<faulty_eval>` should raise an exception matching "<exception_regex>"

  Examples: Exception Strings
      | faulty_eval | exception_regex                     |
      | foo         | undefined local variable or method  |
      | 1 + 'foo'   | String can't be coerced into Fixnum |
      | 1 / 0       | divided by 0                        |


  Scenario: RbContext wrappers should not share scope between instances
     When we create a new RbContext
      And we populate the value of foo with string "bar"
      And we tear down the RbContext
      And we create a new RbContext
     Then attempting to eval `foo` should raise an exception

  Scenario: RbContext wrappers should be able to modify methods from the Ruby side
     When we create a new RbContext
      And we populate the value of foo with string "foo"
      And we evaluate the value of string `foo += "bar"`
      Then the value of `foo` should be string "foobar"


