<?php
error_reporting(E_ALL);
ini_set("display_errors", 1);

include_once('config.inc');

include_once('Converter.inc');
include_once('IntroSpection.inc');
include_once('UserDB.inc');

$users=UserDB::create($dbconfig);

function debugOutput($str) {
  header('Content-Type: text/plain');
  print_r($str);
}

function errorPage($code, $headerlist=null, $content=null) {
  http_response_code($code);
  if ($headerlist) {
    foreach($headerlist as $header => $value) {
      header("${header}: ${value}");
    }
  }
  if (!$content)
    print $code;
  else
    print $content;
  exit(1);
}

function removePrefixIfExists($prefix, $str) {
  if (substr($str, 0, strlen($prefix)) != $prefix)
    return null;
  return substr($str, strlen($prefix));
}

function parseArguments($base='/') {
  global $users;
  $args = removePrefixIfExists($base,   $_SERVER['REQUEST_URI']);
  if (!$args) {
    return new DefaultPage();
  }
  $argv=array_filter(explode('/', $args));
  if ($argv[0] == 'list') {
    return ListResults::createList(array_slice($argv, 1), array_slice($argv, 0, 1),$base);
  }
  elseif ($argv[0] == 'module') {
    if (! in_array($argv[1], Converter::getAllModules())) {
      echo 'No such module';
    }
    else {
      if (count($argv) > 2 && $argv[2] == 'list') {
	return ListResults::createList(array_slice($argv, 3), array_slice($argv, 0, 3), $base, $argv[1]);
	}
      else {
	if (count($argv)>3) {
	  errorPage(500);
	}
	if ($_SERVER['REQUEST_METHOD'] != 'POST' ) {
	  errorPage(405);
	}

	$timeout=0;
	if (!$users->checkApiKey( (array_key_exists('apikey', $_POST)) ? $_POST['apikey'] : null, $timeout)){
	  errorPage(403);
	}
	elseif ($timeout) {
	  errorPage(429,
		    array('Retry-After' => $timeout, 'Content-Type' => 'text/plain'),
		    "429 Too many requests.\n\nRetry after ${timeout} seconds.\n"
		    );
	}


	$in_name= $_FILES['file']['tmp_name'];
	$in_type = $_FILES['file']['type'];
	$targets= $argv[1]::getTargets($in_type, $argv[1]);

	if (array_key_exists($argv[2], $targets)) {
	  $out_type=$targets[$argv[2]];
	}
	else {
	  errorPage(404);
	}
      }
      header('X-InType: '. $in_type);
      header('X-OutType: '. $out_type['mime']);

      return $argv[1]::createConverter($in_name, $in_type, $out_type['mime']);
    }
    exit(1);
  }
  else {
    if (count($argv)>1) {
      errorPage(500);
    }
    if ($_SERVER['REQUEST_METHOD'] != 'POST' ) {
      errorPage(405);
    }

    $timeout=0;
    if (!$users->checkApiKey( (array_key_exists('apikey', $_POST)) ? $_POST['apikey'] : null , $timeout)) {
      errorPage(403);
    }
    elseif ($timeout) {
	  errorPage(429,
		    array('Retry-After' => $timeout, 'Content-Type' => 'text/plain'),
		    "429 Too many requests.\n\nRetry after ${timeout} seconds.\n"
		    );
    }

    $in_name= $_FILES['file']['tmp_name'];
    $in_type = $_FILES['file']['type'];
    $targets=Converter::getAllTargets();

    if (array_key_exists($argv[0], $targets)) {
      $out_type=$targets[$argv[0]];
    }
    else {
      errorPage(404);
    }
  }
  header('X-InType: '. $in_type);
  header('X-OutType: '. $out_type['mime']);

  return Converter::getConverter($in_name, $in_type, $out_type['mime']);
}

$converter=parseArguments('/convert/');

if (!$converter) {
    errorPage(415);
}

$body=$converter->getBody();
//Must be called after getBody, as getBody may modify headers on error
$converter->setHeaders();
print $body;
?>
