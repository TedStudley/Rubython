require 'bundler'
Bundler.setup

require 'rspec/core/rake_task'
RSpec::Core::RakeTask.new(:spec)

require 'rake/extensiontask'
Rake::ExtensionTask.new('rubython') do |ext|
  ext.name = 'rubython_ext'
  ext.ext_dir = 'ext/rubython'
  ext.lib_dir = 'lib/rubython'
end

task :console do
  require 'pry'
  require 'rubython'
  ARGV.clear
  Pry.start
end
