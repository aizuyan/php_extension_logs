<?php
$s = microtime(true);
$logs = Mylogs::getInstance();
$logs->logInit(1, 50, 20480000, "/tmp/", "testlog");

for($i=0; $i<100000; $i++)
{
 	//$logs->test();
	$logs->err("ERR info!!!\n");    
	$logs->msg("MSG info!!!\n");
	$logs->debug("DEBUG info!!!\n");
}

$e = microtime(true);

var_dump($e - $s);
