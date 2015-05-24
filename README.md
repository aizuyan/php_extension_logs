### 一个记录日志的PHP扩展类库 ###
一个记录日志的PHP扩展类库，实现的类和这个地方的基本一致 [PHP滚动日](http://www.cnblogs.com/iforever/p/4498613.html),单例模式的。

### 使用简单 ###
```php
$logs = Mylogs::getInstance();

/*
logInit($level, $maxFileNum, $maxFileSize, $path, $fileName);
$level => 记录的级别，分3个debug(0)，msg(1)，err(2)记录大于等于$level的
$maxFileNum => 最大日志文件数
$maxFileSie => 单个日志文件的最大字节数
$path => 日志文件保存目录
$fileName => 日志文件的名字
*/

$logs->logInit(1, 50, 20480000, "/tmp/", "testlog");

$logs->err("ERR info!!!");    
$logs->msg("MSG info!!!");
$logs->debug("DEBUG info!!!");
```

### 测试文件 ###
测试文件是tests目录下

	tests/extension.php		//使用php扩展
	tests/no-extension.php	//未使用php扩展