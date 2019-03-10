<?php
error_reporting(E_ALL);
ini_set("display_errors", 1);

include_once('Converter.inc');
include_once('IntroSpection.inc');

function errorPage($code) {
  http_response_code($code);
  print $code;
  exit(1);
  //include('my_404.php');
}

function removePrefixIfExists($prefix, $str) {
  if (substr($str, 0, strlen($prefix)) != $prefix)
    return null;
  return substr($str, strlen($prefix));
}

function parseArguments($base='/') {
  $args = removePrefixIfExists($base,   $_SERVER['REQUEST_URI']);
  if (!$args) {
    return new DefaultPage();
  }
  $argv=array_filter(explode('/', $args));
  if ($argv[0] == 'list') {
    return ListResults::createList(array_slice($argv, 1), array_slice($argv, 0, 1),$base);
  }
  else {
    if (count($argv)>1) {
      errorPage(500);
    }
    if ($_SERVER['REQUEST_METHOD'] != 'POST' ) {
      errorPage(405);
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
  header('X-OutType: '. $out_type);

  return Converter::getConverter($in_name, $in_type, $out_type);
}

$converter=parseArguments('/convert/');

if (!$converter) {
    errorPage(415);
}

$converter->setHeaders();
print $converter->getBody();
?>
