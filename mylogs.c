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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mylogs.h"

/* If you declare any globals in php_mylogs.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(mylogs)
*/
zend_class_entry * mylogs_ce;
/* True global resources - no need for thread safety here */
static int le_mylogs;

/* {{{ mylogs_functions[]
 *
 * Every user visible function must have an entry in mylogs_functions[].
 */
const zend_function_entry mylogs_functions[] = {
	PHP_FE_END	/* Must be the last line in mylogs_functions[] */
};
/* }}} */

/* {{{ mylogs_module_entry
 */
zend_module_entry mylogs_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mylogs",
	mylogs_functions,
	PHP_MINIT(mylogs),
	PHP_MSHUTDOWN(mylogs),
	PHP_RINIT(mylogs),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(mylogs),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(mylogs),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_MYLOGS_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MYLOGS
ZEND_GET_MODULE(mylogs)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("mylogs.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_mylogs_globals, mylogs_globals)
    STD_PHP_INI_ENTRY("mylogs.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_mylogs_globals, mylogs_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_mylogs_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_mylogs_init_globals(zend_mylogs_globals *mylogs_globals)
{
	mylogs_globals->global_value = 0;
	mylogs_globals->global_string = NULL;
}
*/
/* }}} */

/**{{{
*/
zval *_getInstance(void) {
	zval *instance;

	instance = zend_read_static_property(mylogs_ce, ZEND_STRL(MYLOGS_INSTANCE), 0 TSRMLS_CC);
	if(IS_OBJECT == Z_TYPE_P(instance)
		&& instanceof_function(Z_OBJCE_P(instance), mylogs_ce TSRMLS_CC)) {
		return instance;
	}

	MAKE_STD_ZVAL(instance);
	object_init_ex(instance, mylogs_ce);
	zend_update_static_property(mylogs_ce, ZEND_STRL(MYLOGS_INSTANCE), instance TSRMLS_CC);
	return instance;
}
/* }}} */

void _efree_char(char *str) {
	if(str != NULL) {
		efree(str);
	}
}

char *_formatTime() {
	struct timeval tv = {0};
	struct tm *timeinfo;
	time_t rawtime;
	char *ret;

	gettimeofday(&tv, NULL);
	rawtime = time(NULL);
	timeinfo = localtime(&rawtime);
	spprintf(&ret, 0, "[%4d-%02d-%02d %02d:%02d:%02d %d.%d]", 
		1900 + timeinfo->tm_year, 
		1 + timeinfo->tm_mon, 
		timeinfo->tm_mday, 
		timeinfo->tm_hour, 
		timeinfo->tm_min, 
		timeinfo->tm_sec, 
		tv.tv_sec, 
		tv.tv_usec);
	timeinfo = NULL;
	return ret;
}

/**{{{
*/
int _log(char *str) {
	zval *logPath, *fileName, *maxSize, *maxNum, *instance;
	char *fileHandler = NULL, *fileHandlerTmp = NULL, *fileHandlerTmpNew = NULL, *format_time = NULL, *format_str = NULL;
	struct stat buf;
	FILE *fp = NULL;

	instance = _getInstance();
	maxNum = zend_read_property(mylogs_ce, instance, ZEND_STRL(MYLOGS_MAXFILE_NUM), 0 TSRMLS_CC);
	maxSize = zend_read_property(mylogs_ce, instance, ZEND_STRL(MYLOGS_MAXFILE_SIZE), 0 TSRMLS_CC);
	logPath = zend_read_property(mylogs_ce, instance, ZEND_STRL(MYLOGS_PATH), 0 TSRMLS_CC);
	fileName = zend_read_property(mylogs_ce, instance, ZEND_STRL(MYLOGS_FILE_NAME), 0 TSRMLS_CC);

	format_time = _formatTime();

	spprintf(&fileHandler, 0, "%s%s.log", Z_STRVAL_P(logPath), Z_STRVAL_P(fileName));

	spprintf(&format_str, 0, "%s %s\n", format_time, str);
	_efree_char(format_time);


	if(VCWD_ACCESS(fileHandler, 0) == F_OK)
	{
		stat(fileHandler, &buf);
		if(buf.st_size >= Z_LVAL_P(maxSize))
		{
			int index = 1;
			for(; index < Z_LVAL_P(maxNum); index++)
			{
				spprintf(&fileHandlerTmp, 0, "%s%s_%d.log", Z_STRVAL_P(logPath), Z_STRVAL_P(fileName), index);
				if(VCWD_ACCESS(fileHandlerTmp, 0) != F_OK) {
					break;
				} else {
					_efree_char(fileHandlerTmp);
				}
			}

			if(index == Z_LVAL_P(maxNum))
				index--;
			for(; index >= 1; index--)
			{
				spprintf(&fileHandlerTmpNew, 0, "%s%s_%d.log", Z_STRVAL_P(logPath), Z_STRVAL_P(fileName), index);
				spprintf(&fileHandlerTmp, 0, "%s%s_%d.log", Z_STRVAL_P(logPath), Z_STRVAL_P(fileName), (index - 1));	
				VCWD_RENAME(fileHandlerTmp, fileHandlerTmpNew);
				_efree_char(fileHandlerTmp);
				_efree_char(fileHandlerTmpNew);
			}
			spprintf(&fileHandlerTmpNew, 0, "%s%s_1.log", Z_STRVAL_P(logPath), Z_STRVAL_P(fileName));
			VCWD_RENAME(fileHandler, fileHandlerTmpNew);
			_efree_char(fileHandlerTmpNew);
		}
	}

	if(fileHandler != NULL) {
		fp = VCWD_FOPEN(fileHandler, "a+b");
		efree(fileHandler);
	}

	if(fp == NULL)
	{
		_efree_char(format_str);
		return FAILURE;
	}
	fputs(format_str, fp);
	_efree_char(format_str);
	fclose(fp);
	return SUCCESS;
}
/* }}} */

/** {{{ mylogs_methods
*/
zend_function_entry mylogs_methods[] = {
	PHP_ME(Mylogs, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
	PHP_ME(Mylogs, getInstance, NULL, ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	PHP_ME(Mylogs, logInit, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Mylogs, debug, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Mylogs, msg, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Mylogs, err, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(Mylogs, test, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};
/**}}}
*/

PHP_METHOD(Mylogs, test) {
	php_printf("%s\n", _formatTime());
}
/**{{{ proto Mylogs::__construct
*/
PHP_METHOD(Mylogs, __construct) {}
/**}}}
*/

/**{{{ proto Mylogs::getInstance
* singleton pattern, to get the instance
*/
PHP_METHOD(Mylogs, getInstance) {
	zval *instance = _getInstance();
	RETURN_ZVAL(instance, 1, 0);
}
/**}}}
*/

/**{{{ proto Mylogs::logInit
*/
PHP_METHOD(Mylogs, logInit) {
	long level, maxNum, maxSize;
	char  *logPath, *fileName;
	int logPath_len, fileName_len;
	zval *instance = _getInstance();
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllss", &level, &maxNum, &maxSize, &logPath, &logPath_len, &fileName, &fileName_len) == FAILURE)
		return;

	zend_update_property_long(mylogs_ce, instance, ZEND_STRL(MYLOGS_LEVEL), level TSRMLS_CC);
	zend_update_property_long(mylogs_ce, instance, ZEND_STRL(MYLOGS_MAXFILE_NUM), maxNum TSRMLS_CC);
	zend_update_property_long(mylogs_ce, instance, ZEND_STRL(MYLOGS_MAXFILE_SIZE), maxSize TSRMLS_CC);
	zend_update_property_string(mylogs_ce, instance, ZEND_STRL(MYLOGS_PATH), logPath TSRMLS_CC);
	zend_update_property_string(mylogs_ce, instance, ZEND_STRL(MYLOGS_FILE_NAME), fileName TSRMLS_CC);

	RETURN_TRUE;
}
/**}}}
*/

/** {{{ Mylogs::debug
*/
PHP_METHOD(Mylogs, debug) {
	char *msg;
	int msg_len;
	zval *instance, *level, **_level;
	zend_class_entry *ce;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &msg, &msg_len) == FAILURE)
		return;	

	instance = _getInstance();
	ce = Z_OBJCE_P(instance);
	level = zend_read_property(mylogs_ce, instance, ZEND_STRL(MYLOGS_LEVEL), 0 TSRMLS_CC);
	zend_hash_find(&ce->constants_table, ZEND_STRS(LOG_DEBUG), (void **)&_level);

	if(Z_LVAL_P(level) > Z_LVAL_PP(_level)) {
		RETURN_TRUE;
	}

	if(_log(msg) == SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/*}}}*/

/** {{{ Mologs::msg
*/
PHP_METHOD(Mylogs, msg) {
	char *msg;
	int msg_len;
	zval *instance, *level, **_level;
	zend_class_entry *ce;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &msg, &msg_len) == FAILURE)
		return;	

	instance = _getInstance();
	ce = Z_OBJCE_P(instance);
	level = zend_read_property(mylogs_ce, instance, ZEND_STRL(MYLOGS_LEVEL), 0 TSRMLS_CC);
	zend_hash_find(&ce->constants_table, ZEND_STRS(LOG_MSG), (void **)&_level);

	if(Z_LVAL_P(level) > Z_LVAL_PP(_level)) {
		RETURN_TRUE;
	}

	if(_log(msg) == SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/** {{{ Mylogs::err
*/
PHP_METHOD(Mylogs, err) {
	char *msg;
	int msg_len;
	zval *instance, *level, **_level;
	zend_class_entry *ce;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &msg, &msg_len) == FAILURE)
		return;	

	instance = _getInstance();
	ce = Z_OBJCE_P(instance);
	level = zend_read_property(mylogs_ce, instance, ZEND_STRL(MYLOGS_LEVEL), 0 TSRMLS_CC);
	zend_hash_find(&ce->constants_table, ZEND_STRS(LOG_ERR), (void **)&_level);

	if(Z_LVAL_P(level) > Z_LVAL_PP(_level)) {
		RETURN_TRUE;
	}

	if(_log(msg) == SUCCESS) {
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mylogs)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Mylogs", mylogs_methods);

	mylogs_ce = zend_register_internal_class_ex(&ce, NULL, NULL TSRMLS_CC);

	zend_declare_property_null(mylogs_ce, ZEND_STRL(MYLOGS_LEVEL), ZEND_ACC_PRIVATE TSRMLS_CC);	
	zend_declare_property_null(mylogs_ce, ZEND_STRL(MYLOGS_MAXFILE_NUM), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(mylogs_ce, ZEND_STRL(MYLOGS_MAXFILE_SIZE), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(mylogs_ce, ZEND_STRL(MYLOGS_PATH), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(mylogs_ce, ZEND_STRL(MYLOGS_FILE_NAME), ZEND_ACC_PRIVATE TSRMLS_CC);

	zend_declare_property_null(mylogs_ce, ZEND_STRL(MYLOGS_INSTANCE),  ZEND_ACC_PRIVATE|ZEND_ACC_STATIC TSRMLS_CC);


	zend_declare_class_constant_long(mylogs_ce, ZEND_STRL(LOG_DEBUG), 0 TSRMLS_CC);
	zend_declare_class_constant_long(mylogs_ce, ZEND_STRL(LOG_MSG), 1 TSRMLS_CC);
	zend_declare_class_constant_long(mylogs_ce, ZEND_STRL(LOG_ERR), 2 TSRMLS_CC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mylogs)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mylogs)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mylogs)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mylogs)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mylogs support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
