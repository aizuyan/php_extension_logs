### 一个记录日志的PHP扩展类库 ###
一个记录日志的PHP扩展类库，实现的类和这个地方的基本一致 [PHP滚动日](http://www.cnblogs.com/iforever/p/4498613.html),单例模式的。

### 使用简单 ###
```php
$logs = Mylogs::getInstance();
$logs->logInit(1, 50, 20480000, "/tmp/", "testlog");

$logs->err("ERR info!!!");    
$logs->msg("MSG info!!!");
$logs->debug("DEBUG info!!!");
```

### 测试文件 ###
测试文件是tests目录下

	tests/extension.php		//使用php扩展
	tests/no-extension.php	//未使用php扩展