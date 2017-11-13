Feature: RbObject Functionality
  The RbObject class should provide a strong reference to an Object within Ruby

  Background: The RbContext instance should be cleared before each test
    Given that the test environment is initialized with no active RbContext instance
      And that we create a new RbContext

  Scenario: We should be able to exfiltrate an anonymous Object instance from Ruby
     When we evaluate `Object.new` within the RbContext
     Then the result should be a valid RbObject

  Scenario: We should be able to call methods from Ruby on an exfiltrated Ruby Object instance
     When we evaluate `Object.new` within the RbContext
     Then the result should be a valid RbObject
      And the result of calling `to_s` on that object should match "^#<Object"

  Scenario: We should be able to extract more complex objects from Ruby
     When we evaluate `binding` within the RbContext
     Then the result should be a valid RbObject

  Scenario: We should be able to call methods from Ruby on an exfiltrated Ruby Object
     When we evaluate `binding` within the RbContext
     Then the result should be a valid RbObject
      And the result of calling `local_variables` on the saved object should be "[]"

  Scenario: Exfiltrated Ruby Objects Should be Affected by Changes Within Ruby
     When we evaluate `binding` within the RbContext and save the result
      And we save the result of calling `local_variables` on the saved object as "first"
      And we evaluate `foo = 'bar'` within the RbContext
      And we save the result of calling `local_variables` on the saved object as "second"
     Then the string values of saved results "first" and "second" should be distinct
      And the string value of saved result "first" should be "[]"
      And the string value of saved result "second" should be "[:foo]"
