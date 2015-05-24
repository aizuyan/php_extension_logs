dnl $Id$
dnl config.m4 for extension mylogs

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(mylogs, for mylogs support,
Make sure that the comment is aligned:
[  --with-mylogs             Include mylogs support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(mylogs, whether to enable mylogs support,
dnl Make sure that the comment is aligned:
dnl [  --enable-mylogs           Enable mylogs support])

if test "$PHP_MYLOGS" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-mylogs -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/mylogs.h"  # you most likely want to change this
  dnl if test -r $PHP_MYLOGS/$SEARCH_FOR; then # path given as parameter
  dnl   MYLOGS_DIR=$PHP_MYLOGS
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for mylogs files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       MYLOGS_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$MYLOGS_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the mylogs distribution])
  dnl fi

  dnl # --with-mylogs -> add include path
  dnl PHP_ADD_INCLUDE($MYLOGS_DIR/include)

  dnl # --with-mylogs -> check for lib and symbol presence
  dnl LIBNAME=mylogs # you may want to change this
  dnl LIBSYMBOL=mylogs # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MYLOGS_DIR/lib, MYLOGS_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_MYLOGSLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong mylogs lib version or lib not found])
  dnl ],[
  dnl   -L$MYLOGS_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(MYLOGS_SHARED_LIBADD)

  PHP_NEW_EXTENSION(mylogs, mylogs.c, $ext_shared)
fi
