<?php
abstract class UserDB {
  abstract function checkApiKey($apikey, &$timeout=0);

  static $dbm = array();
  static function create($dbstring) {
    $elems=explode(':', $dbstring, 2);
    $class=self::$dbm[$elems[0]];
    $db = new $class($elems[1]);
    return $db;
  }

  static protected function registerDB($prefix, $class) {
    self::$dbm[$prefix]=$class;
  }

  static protected function validateHash($apitransitkey, $apikey) {
    $elems=explode('-', $apitransitkey, 2);
    $algo='sha1';

    $date=new DateTime();
    if ($elems[1]==hash($algo, $apikey . '-' .$date->format('Y-m-d'))) {
      return true;
    }
    
    $date->add(DateInterval::createFromDateString('yesterday'));
    if ($apitransitkey==hash($algo, $apikey . '-' .$date->format('Y-m-d'))) {
      return true;
    }
    
    $date=new DateTime();
    $date->add(DateInterval::createFromDateString('tomorrow'));
    if ($apitransitkey==hash($algo, $apikey . '-' .$date->format('Y-m-d'))) {
      return true;
    }
    return false;
  }

  static protected function getUser($apitransitkey) {
    if (!$apitransitkey)
      return null;
    $elems=explode('-', $apitransitkey, 2);
    return $elems[0];
  }
}

class NullDB extends UserDB {
  function __construct($location) {
  }
  
  function checkApiKey($apikey, &$timeout=0) {
    $timeout = 0;
    return true;
  }

  static function init() {
    self::registerDB("nulldb", __CLASS__);
  }
}

NullDB::init();
?>