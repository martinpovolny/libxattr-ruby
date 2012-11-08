
require "xattr"
require "test/unit"

class XAttrTest < Test::Unit::TestCase
  TEST_PATH='/tmp/rtest'

  def test_all
    
    XAttr::set( TEST_PATH, 'user.ruby', 'rulez')
    XAttr::lset( TEST_PATH, 'user.foo', 'bar' )

    attr = nil
    XAttr::list(TEST_PATH) { |a| attr = a }
    assert_instance_of( String, attr )

    attr = nil
    XAttr::llist(TEST_PATH) { |a| attr = a }
    assert_instance_of( String, attr )

    assert_equal( XAttr::get(TEST_PATH, 'user.ruby'), 'rulez' )
    assert_equal( XAttr::lget(TEST_PATH, 'user.foo'), 'bar' )

    XAttr::remove(TEST_PATH, 'user.ruby')
    XAttr::lremove(TEST_PATH, 'user.foo')

    assert_raise( Errno::ENODATA ) { XAttr::get(TEST_PATH, 'user.ruby') }
    assert_raise( Errno::ENODATA ) { XAttr::lget(TEST_PATH, 'user.foo') }
  end
end
