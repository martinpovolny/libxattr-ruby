#include <ruby.h>

#include <sys/types.h>
#include <attr/xattr.h>

VALUE cXAttr;

/*
 * Set extended attribute on file or fd (follows links)
 *
 *  optional flags may be XAttr::XATTR_REPLACE or XAttr::XATTR_CREATE
 *  when not given attribute will be replaced or created as needed
 * 
 * call-seq:
 *   XAttr::set( file, name, value, flags )
 *   XAttr::set( file, name, value )
 *   XAttr::set( fd, name, value )
 *
 */ 
VALUE ruby_xattr_set( int argc, VALUE *argv, VALUE class ) {
  int flags, ret;

  switch (argc) {
    case 4:
      flags = FIX2INT( argv[3] );
      break;
    case 3:
      flags = 0;
      break;
    default:
      rb_raise(rb_eArgError, "wrong number of arguments (expect 3 or 4)");
  }
        
  if (FIXNUM_P(argv[0])) {
    ret = fsetxattr( FIX2INT(argv[0]),      RSTRING(argv[1])->ptr, 
                     RSTRING(argv[2])->ptr, RSTRING(argv[2])->len, flags );
  } else {
    ret = setxattr(  RSTRING(argv[0])->ptr, RSTRING(argv[1])->ptr, 
                     RSTRING(argv[2])->ptr, RSTRING(argv[2])->len, flags );
  }
  if (0 != ret) {
    rb_sys_fail("[f]setxattr(file,name,val,flags) failed");
  }
  return argv[2];
}

/*
 * Set extended attribute on file (doesn't follow links)
 *
 *  optional flags may be XAttr::XATTR_REPLACE or XAttr::XATTR_CREATE
 *  when not given attribute will be replaced or created as needed
 * 
 * call-seq:
 *   XAttr::lset( file, name, value, flags )
 *   XAttr::lset( file, name, value )
 *
 */ 
VALUE ruby_xattr_lset( int argc, VALUE *argv, VALUE class ) {
  int flags;

  switch (argc) {
    case 4:
      flags = FIX2INT( argv[3] );
      break;
    case 3:
      flags = 0;
      break;
    default:
      rb_raise(rb_eArgError, "wrong number of arguments (expect 3 or 4)");
  }
        
  if (0 != lsetxattr( RSTRING(argv[0])->ptr, RSTRING(argv[1])->ptr, 
                      RSTRING(argv[2])->ptr, RSTRING(argv[2])->len, flags ) ) {
    rb_sys_fail("lsetxattr(file,name,val,flags) failed");
  }
  return argv[2];
}

/*
 * Get extended attribute of file or fd (follows links)
 *
 * call-seq:
 *   XAttr::get( file, name ) -> string
 *   XAttr::get( fd, name )   -> string
 *
 */ 
VALUE ruby_xattr_get( VALUE class, VALUE file, VALUE name ) {
  ssize_t size;
  char *buff;
  VALUE res;

  // ssize_t getxattr (const char *path, const char *name, void *value, size_t size);
  if (FIXNUM_P(file)) {
    size = fgetxattr( FIX2INT(file),     RSTRING(name)->ptr, NULL, 0 );
  } else {
    size = getxattr( RSTRING(file)->ptr, RSTRING(name)->ptr, NULL, 0 );
  }
  if (-1 == size) {
    rb_sys_fail("[f]getxattr(file,name,NULL,0) failed");
  }

  buff = (char*)malloc(size+1);
  if (FIXNUM_P(file)) {
    size = fgetxattr( FIX2INT(file),     RSTRING(name)->ptr, buff, size );
  } else {
    size = getxattr( RSTRING(file)->ptr, RSTRING(name)->ptr, buff, size );
  }
  if (-1 == size) {
    free(buff);
    rb_sys_fail("[f]getxattr(file,name,buff,size) failed");
  }
  
  res = rb_str_new(buff, size);
  free(buff);
  return res;
}

/*
 * Get extended attribute of file (doesn't follow links)
 *
 * call-seq:
 *   XAttr::lget( file, name ) -> string
 *
 */ 
VALUE ruby_xattr_lget( VALUE class, VALUE file, VALUE name ) {
  ssize_t size;
  char *buff;
  VALUE res;

  size = lgetxattr( RSTRING(file)->ptr, RSTRING(name)->ptr, NULL, 0 );
  if (-1 == size) {
    rb_sys_fail("lgetxattr(file,name,NULL,0) failed");
  }

  buff = (char*)malloc(size+1);
  size = lgetxattr( RSTRING(file)->ptr, RSTRING(name)->ptr, buff, size );
  if (-1 == size) {
    free(buff);
    rb_sys_fail("lgetxattr(file,name,buff,size) failed");
  }
  
  res = rb_str_new(buff, size);
  free(buff);
  return res;
}

/*
 *  Iterate over list of attrs of given file or fd
 *  (follows symlinks).
 *
 *  call-seq:
 *    XAttr::list( file ) { |name| }
 *    XAttr::list( fd )   { |name| }
 *
 */
VALUE ruby_xattr_list( VALUE class, VALUE file ) {
  ssize_t size;
  char *buff, *ptr;
  VALUE res;

  if (FIXNUM_P(file)) {
    size = flistxattr( FIX2INT(file),     NULL, 0 );
  } else {
    size = listxattr( RSTRING(file)->ptr, NULL, 0 );
  }
  if (-1 == size) {
    rb_sys_fail("[f]listxattr(file,NULL,0) failed");
  }
  buff = (char*)malloc(size+1);

  if (FIXNUM_P(file)) {
    size = flistxattr( FIX2INT(file),     buff, size );
  } else {
    size = listxattr( RSTRING(file)->ptr, buff, size );
  }
  if (-1 == size) {
    free(buff);
    rb_sys_fail("[f]listxattr(file,buff,size) failed");
  }
  
  ptr = buff;
  /* cycle over attrs, call yield for each */
  while ( ptr < buff+size ) {
    res = rb_str_new2( ptr );
    rb_yield( res );
    while (*ptr != 0) ptr++;
    ptr++;
  }
  free(buff);

  return Qtrue; /* what should we return? */
}

/*
 *  Iterate over list of attrs of given file
 *  (doesn't follow symlinks).
 *
 *  call-seq:
 *    XAttr::list( file ) { |name| }
 *    XAttr::list( fd )   { |name| }
 *
 */
VALUE ruby_xattr_llist( VALUE class, VALUE file ) {
  ssize_t size;
  char *buff, *ptr;
  VALUE res;

  size = llistxattr( RSTRING(file)->ptr, NULL, 0 );
  if (-1 == size) {
    rb_sys_fail("llistxattr(file,NULL,0) failed");
  }
  buff = (char*)malloc(size+1);

  size = llistxattr( RSTRING(file)->ptr, buff, size );
  if (-1 == size) {
    free(buff);
    rb_sys_fail("llistxattr(file,buff,size) failed");
  }
  
  ptr = buff;
  /* cycle over attrs, call yield for each */
  while ( ptr < buff+size ) {
    res = rb_str_new2( ptr );
    rb_yield( res );
    while (*ptr != 0) ptr++;
    ptr++;
  }
  free(buff);

  return Qtrue; /* what should we return? */
}

/*
 *  Remove attribute name from file or fd
 *  (follows symlinks).
 *
 *  call-seq:
 *    XAttr::remove( file, name )
 *    XAttr::remove( fd, name )
 *
 */
VALUE ruby_xattr_remove( VALUE class, VALUE file, VALUE name ) {
  int res;

  if (FIXNUM_P(file)) {
    res = fremovexattr( FIX2INT(file),     RSTRING(name)->ptr );
  } else {
    res = removexattr( RSTRING(file)->ptr, RSTRING(name)->ptr );
  }
  if (-1 == res) {
    rb_sys_fail("[f]removexattr(file,buff) failed");
  }
  return Qtrue;
}

/*
 *  Remove attribute name from file
 *  (doesn't follow symlinks).
 *
 *  call-seq:
 *    XAttr::lremove( file, name )
 *
 */
VALUE ruby_xattr_lremove( VALUE class, VALUE file, VALUE name ) {
  if (-1 == lremovexattr( RSTRING(file)->ptr, RSTRING(name)->ptr ) ) {
    rb_sys_fail("lremovexattr(file,buff) failed");
  }
  return Qtrue;
}

/*
 *  Document-class: XAttr
 *  XAttr -- interface to extended attributes
 */
void Init_xattr(void) {
  cXAttr   = rb_define_class( "XAttr", rb_cObject );

  rb_define_singleton_method( cXAttr, "set",     ruby_xattr_set,    -1 );
  rb_define_singleton_method( cXAttr, "lset",    ruby_xattr_lset,   -1 );
  rb_define_singleton_method( cXAttr, "get",     ruby_xattr_get,     2 );
  rb_define_singleton_method( cXAttr, "lget",    ruby_xattr_lget,    2 );
  rb_define_singleton_method( cXAttr, "remove",  ruby_xattr_remove,  2 );
  rb_define_singleton_method( cXAttr, "lremove", ruby_xattr_lremove, 2 );
  rb_define_singleton_method( cXAttr, "list",    ruby_xattr_list,    1 );
  rb_define_singleton_method( cXAttr, "llist",   ruby_xattr_llist,   1 );

  rb_define_const( cXAttr, "XATTR_CREATE",  INT2FIX(XATTR_CREATE)  );
  rb_define_const( cXAttr, "XATTR_REPLACE", INT2FIX(XATTR_REPLACE) );
}

