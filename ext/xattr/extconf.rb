require 'mkmf'

have_library('attr','setxattr')
create_makefile("xattr")

rdoc = find_executable0( 'rdoc' ) || find_executable0( 'rdoc1.8' )

File.open('Makefile', "a") do |f|
  f << <<EOS
doc: xattr.c xattr.rb
	#{rdoc} $^

dist: xattr.c extconf.rb README copyright t/test.rb
	mkdir -p xattr-ruby
	cp --parents $^ xattr-ruby/
	tar cvjf xattr.tar.bz2 xattr-ruby/
	rm -rf xattr-ruby

EOS
end
