require File.expand_path('../lib/rubython/version', __FILE__)

Gem::Specification.new do |s|
  s.name            = 'rubython'
  s.version         = Rubython::VERSION
  s.platform        = Gem::Platform::RUBY
  s.date            = '2017-10-21'
  s.summary         = "Rubython"
  s.description     = "I don't even know"
  s.authors         = ["Ted Studley"]
  s.email           = ["estudley@castlighthealth.com"]

  s.files        = Dir["{lib}/**/*.rb", "bin/*", "LICENSE", "*.md"]
  s.require_path = 'lib'

  s.extensions = 'ext/rubython/extconf.rb'
end
