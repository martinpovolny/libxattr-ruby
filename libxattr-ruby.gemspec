require File.expand_path('../lib/xattr/version', __FILE__)

Gem::Specification.new do |s|
  s.name        = "xattr"
  s.version     = XAttr::VERSION
  s.summary     = "Extended Attributes (EA) bindings"
  s.description = "Bindings for Linux Extended Attributes (EA)"
  s.homepage    = "https://github.com/martinpovolny/libxattr-ruby"
  s.authors     = ["Martin Povolny"]
  s.email       = ["martin.povolny@gmail.com"]
  
  #s.add_runtime_dependency 'builder'

  s.files       = `git ls-files '*.c' '*.rb' '*.h'`.split("\n")
  s.extensions  = ['ext/xattr/extconf.rb']

  #s.files         = `git ls-files`.split("\n")
  #s.executables   = `git ls-files -- bin/*`.split("\n").map{|f| File.basename(f)}

  #s.add_dependency('fileutils')
  s.has_rdoc = true
  s.extra_rdoc_files = ['ext/xattr/xattr.c']

  
  s.require_paths = ["lib"]
end
