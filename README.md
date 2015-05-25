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
	
	
### 下面是对应的php类库 ###
```php
class LOGS {
	private $level;
	private $maxFileNum;
	private $maxFileSize;
	private $logPath;
	private $file;

	//日志的级别DEBUG，MSG，ERR
	const LOGS_DEBUG = 0;
	const LOGS_MSG = 1;
	const LOGS_ERR = 2;

	private static $instance = null;

	private function __construct(){}

	public static function getInstance()
	{
		if(self::$instance == null)
		{
			self::$instance = new self();
		}
		return self::$instance;
	}

	/**
	 * @Desc 初始化
	 * @Param $level int 记录级别
	 * @Param $maxNum int 最大日志文件数目
	 * @Param $maxSize int 最大日志文件大小
	 * @Param $logPath string 日志文件保存路径
	 * @Param $file string 日志文件名称前缀
	 * @Return boolean
	 */
	public function init($level, $maxNum, $maxSize, $logPath, $file)
	{
		$this->level = $level;
		$this->maxFileNum = $maxNum;
		$this->maxFileSize = $maxSize;
		$this->logPath = $logPath;
		$this->file = $file;
		return true;
	}

	/**
	 * @Desc 获取格式化时间串
	 */
	public function formatTime()
	{
        $ustime = explode ( " ", microtime () );
        return "[" . date('Y-m-d H:i:s', time()) .".". ($ustime[0] * 1000) . "]";
	}

	/**	
	 * @Desc 滚动方式记录日志文件
	 */
	public function log($str)
	{
		$path = $this->logPath.DIRECTORY_SEPARATOR.$this->file.".log";
		clearstatcache();
		if(file_exists($path))
		{
			if(filesize($path) >= $this->maxFileSize)
			{
				$index = 1;
				//获取最大的滚动日志数目
				for(;$index < $this->maxFileNum; $index++)
				{
					if(!file_exists($this->logPath.DIRECTORY_SEPARATOR.$this->file."_".$index.".log"))
					{
						break;
					}
				}
				//已经存在maxFileNum个日志文件了
				if($index == $this->maxFileNum)
				{
					$index--;
				}
				//滚动日志
				for(;$index > 1; $index--)
				{
					$new = $this->logPath.DIRECTORY_SEPARATOR.$this->file."_".$index.".log";
					$old = $this->logPath.DIRECTORY_SEPARATOR.$this->file."_".($index - 1).".log";
					rename($old, $new);
				}

				$newFile = $this->logPath.DIRECTORY_SEPARATOR.$this->file."_1.log";
				rename($path, $newFile);
			}
		}
		$fp = fopen($path, "a+b");
		fwrite($fp, $str, strlen($str));
		fclose($fp);
		return true;
	}

	/**
	 * @Desc 记录调试信息
	 * @Param string 日志信息
	 */
	public function debug($msg)
	{
		if($this->level <= self::LOGS_DEBUG)
		{
			$this->log($this->formatTime()."DEBUG: ${msg}\n");
		}
	}

	/**
	 * @Desc 记录信息
	 * @Param string 日志信息
	 */
	public function msg($msg)
	{
		if($this->level <= self::LOGS_MSG)
		{
			$this->log($this->formatTime()."MSG: ${msg}\n");
		}
	}

	/**
	 * @Desc 记录错误信息
	 * @Param string 日志信息
	 */
	public function err($msg)
	{
		if($this->level <= self::LOGS_ERR)
		{
			$this->log($this->formatTime()."ERR: ${msg}\n");
		}
	}
}
```
