<?php
$s = microtime(true);
final class LOGS {
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
		$level = intval($level);
		$maxNum = intval($maxNum);
		$maxSize = intval($maxSize);
		!is_dir($logPath) && mkdir($logPath, 0777, true);
		if(!in_array($level, array(self::LOGS_DEBUG, self::LOGS_MSG, self::LOGS_ERR)) || $maxNum <= 0 || $maxSize <= 0 || !is_dir($logPath))
		{
			return false;
		}
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
		$str = $this->formatTime()." {$str}\n";
		fwrite($fp, $str, strlen($str));
		fclose($fp);
		return true;
	}

	/**
	 * @Desc 记录调试信息
	 * @Param string 日志信息
	 * @Param string 日志所在文件
	 * @Param string 日志所在行
	 */
	public function debug($msge)
	{
		if($this->level <= self::LOGS_DEBUG)
		{
			$this->log("${msg}");
		}
	}

	/**
	 * @Desc 记录信息
	 * @Param string 日志信息
	 * @Param string 日志所在文件
	 * @Param string 日志所在行
	 */
	public function msg($msg)
	{
		if($this->level <= self::LOGS_MSG)
		{
			$this->log("${msg}");
		}
	}

	/**
	 * @Desc 记录错误信息
	 * @Param string 日志信息
	 * @Param string 日志所在文件
	 * @Param string 日志所在行
	 */
	public function err($msg)
	{
		if($this->level <= self::LOGS_ERR)
		{
			$this->log("${msg}");
		}
	}
}

date_default_timezone_set('PRC');

$logs = LOGS::getInstance();
$logs->init(1, 50, 20480000, "/tmp/", "yrtphp");

for($i=0; $i<100000; $i++)
{
 	//$logs->test();
	$logs->err("ERR info!!!\n");    
	$logs->msg("MSG info!!!\n");
	$logs->debug("DEBUG info!!!\n");
}
$e = microtime(true);

var_dump($e - $s);
