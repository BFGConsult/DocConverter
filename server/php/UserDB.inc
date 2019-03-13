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

class SQLiteUserDB extends UserDB {
  
  function __construct($location) {
    $db = new SQLite3($location);

    $forceInit = false;

    if ($forceInit)
      $db->query('DROP TABLE Users;');

    $db->query('CREATE TABLE IF NOT EXISTS Users (user varchar(10) PRIMARY KEY, apikey char(40), ratelimit INTEGER, tokens INTEGER, atime INTEGER )');

    if ($forceInit)
      $db->query('INSERT INTO Users VALUES (NULL, NULL, 10, 10, strftime("%s", "now"));');

    $this->db = $db;
  }
  
  function checkApiKey($apitransitkey, &$timeout=0) {
    $uname=$this->getUser($apitransitkey);

    $query='SELECT * FROM Users WHERE user ';
    $query.=($uname)?' = "' . $uname .'";':' IS NULL;';

    $user = $this->db->querySingle($query, true);

    if ($uname && (!$user || !$this->validateHash($apitransitkey, $user['apikey'])))
      return false;

    
    $sPerTok=3600/$user['ratelimit'];
    if ($user['tokens'] < $user['ratelimit']) {
      $newTokens=(time()-$user['atime'])/$sPerTok;
      $residual=round(($newTokens-floor($newTokens))*$sPerTok);
      $newTokens=floor($newTokens);
    }
    else {
      $newTokens=0;
      $residual=0;
    }

    $uTokens=$user['tokens'] + $newTokens;
    if ($uTokens > $user['ratelimit'] + 1) {
      $uTokens = $user['ratelimit'] + 1;
      $residual = 0;
    }

    if ($uTokens > 0) {
      $uTokens--;
      $timeout = 0;
    }
    else {
      $timeout = $sPerTok - $residual;
      return true;
    }
    $uatime = time() - $residual;

    $query = 'UPDATE Users SET tokens = ' . $uTokens . ', atime = ' . $uatime . ' WHERE user';
    $query .= ($uname) ? ' = "' . $uname .'";' : ' IS NULL;';

    $res = $this->db->querySingle($query, true);

    return true;
  }

  static function init() {
    self::registerDB("sqlite", __CLASS__);
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


SQLiteUserDB::init();
NullDB::init();
?>