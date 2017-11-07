Feature: Basic Rubython Functionality

  Scenario: Rubython exists
    Given that we have Rubython installed
    When we import rubython
    Then the rubython module should be in scope
