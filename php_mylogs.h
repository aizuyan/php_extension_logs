/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_MYLOGS_H
#define PHP_MYLOGS_H

extern zend_module_entry mylogs_module_entry;
#define phpext_mylogs_ptr &mylogs_module_entry

#define PHP_MYLOGS_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_MYLOGS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_MYLOGS_API __attribute__ ((visibility("default")))
#else
#	define PHP_MYLOGS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define MYLOGS_LEVEL "level"
#define MYLOGS_MAXFILE_NUM "maxFileNum"
#define MYLOGS_MAXFILE_SIZE "maxFIleSize"
#define MYLOGS_PATH "logPath"
#define MYLOGS_FILE_NAME "fileName"
#define MYLOGS_INSTANCE "instance"
#define LOG_DEBUG "LOG_DEBUG"
#define LOG_MSG "LOG_MSG"
#define LOG_ERR "LOG_EE"


PHP_MINIT_FUNCTION(mylogs);
PHP_MSHUTDOWN_FUNCTION(mylogs);
PHP_RINIT_FUNCTION(mylogs);
PHP_RSHUTDOWN_FUNCTION(mylogs);
PHP_MINFO_FUNCTION(mylogs);
zval *_getInstance(void);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(mylogs)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(mylogs)
*/

/* In every utility function you add that needs to use variables 
   in php_mylogs_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as MYLOGS_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MYLOGS_G(v) TSRMG(mylogs_globals_id, zend_mylogs_globals *, v)
#else
#define MYLOGS_G(v) (mylogs_globals.v)
#endif

#endif	/* PHP_MYLOGS_H */

extern zend_class_entry *mylogs_ce;
PHP_METHOD(Mylogs, __construct);
PHP_METHOD(Mylogs, getInstance);
PHP_METHOD(Mylogs, logInit);
PHP_METHOD(Mylogs, debug);
PHP_METHOD(Mylogs, debug);
PHP_METHOD(Mylogs, msg);
PHP_METHOD(Mylogs, err);
PHP_METHOD(Mylogs, test);
int _log(char *str);
zval *_getInstance(void);
char *_formatTime();
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
