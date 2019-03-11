<?php
include_once 'Result.inc';

class DefaultPage extends Result {

  function __construct() {
  }
  
  function getResultType() {
    return 'text/html';
  }
  
  function getBody() {
    //print_r( Converter::getAllTargets());
    return '<!DOCTYPE html>
<html lang="en">
<head>
<title>DocConverter</title>
<script src="../convertFunctions.js"></script>
</head>

<body>
<h1>Document Converter</h1>

<form id="form" method="post" action="" enctype="multipart/form-data">
<p>Source file: <input id="fileinput" name="file" type="file" onchange="setTargets()"/><input id="suffix" name="suffix" value="" type="hidden"/> </p>
<p>
Target file format:
<select id="targets" disabled="true" onchange="setAction()">
</select>
</p>
    <input id="submit" disabled="true" type="submit" value="Convert" />
</form>
<script>
setTargets();
</script>
</body> </html>

';
  }  
}

class ListResults extends Result {
  protected $resultcode;

  function __construct() {
    $this->resultcode = 200;
  }
  
  function getResultType() {
    return 'application/json';
  }
  
  function getResultCode() {
    return $this->resultcode;
  }
  
  function getBody() {
    $res = array('sources', 'targets', 'modules');
    return json_encode($res);
  }

  static function createList($myArgs, $prevArgs, $base) {
    $c = count($myArgs);
    if ($c > 0) {
      $format = ($c > 1) ? $myArgs[1] : null;
      switch ($myArgs[0]) {
      case 'modules':
	return new ListModules();
      case 'targets':
	return new ListTargets($format);
      case 'sources':
	return new ListSources($format);
      }

      $list = new ListResults();
      $list->resultcode = 404;
      return $list;
    }
    else {
      return new ListResults();
    }
  }
}

class ListTargets extends ListResults {
  function __construct($sourceFormat) {
    $this->sourceFormat = $sourceFormat;
  }

  function getBody() {
    $targetList = Converter::getAllTargets($this->sourceFormat);

    //Put selfconversion at the end
    $sourceTarget = $targetList[$this->sourceFormat];
    unset($targetList[$this->sourceFormat]);
    $targetList[$this->sourceFormat]=$sourceTarget;

    return json_encode($targetList);
  }
}

class ListSources extends ListResults {
  function __construct($targetFormat) {
    $this->targetFormat = $targetFormat;
  }

  function getBody() {
    return json_encode(Converter::getAllSources($this->targetFormat));
  }
}
class ListModules extends ListResults {
  function getBody() {
    return json_encode(Converter::getAllModules());
  }
}
?>
