require 'bundler/gem_tasks'
require 'rake/testtask'
require 'rake/extensiontask'

Rake::TestTask.new do |t|
  t.libs << 'lib'
  t.test_files = FileList['specs/**/*_spec.rb']
end

spec = Gem::Specification.load('fast_bitarray.gemspec')
Rake::ExtensionTask.new('fast_bitarray', spec)