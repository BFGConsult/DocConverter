<?php
require_once('UserDB.inc');
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
SQLiteUserDB::init();
?>