MRuby::Gem::Specification.new('mruby-esp32-i2c') do |spec|
  spec.license = 'MIT'
  spec.authors = 'YAMAMOTO Masaya'

  spec.cc.include_paths << "#{build.root}/src"

  spec.add_dependency 'mruby-numeric-ext'
end
