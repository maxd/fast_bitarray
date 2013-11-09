# coding: utf-8

Gem::Specification.new do |spec|
  spec.name          = 'fast_bitarray'
  spec.version       = '0.0.1'
  spec.authors       = ['Maxim Dobryakov']
  spec.email         = %w(maxim.dobryakov@gmail.com)
  spec.description   = %q{Fast BitArray implementation for Ruby}
  spec.summary       = %q{Fast BitArray implementation for Ruby base on C extension}
  spec.homepage      = ''
  spec.license       = 'MIT'

  spec.files         = `git ls-files`.split($/)
  spec.executables   = []
  spec.test_files    = spec.files.grep(%r{^(test|spec|features)/})
  spec.require_paths = %w(lib)
  spec.extensions = %w(ext/fast_bitarray/extconf.rb)

  spec.add_development_dependency 'bundler', '~> 1.3'
  spec.add_development_dependency 'rake'
  spec.add_development_dependency 'rake-compiler'
end
