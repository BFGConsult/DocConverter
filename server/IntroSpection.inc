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
  protected $module;

  function __construct($module=null) {
    $this->resultcode = 200;
    $this->module = $module;
  }
  
  function getResultType() {
    return 'application/json';
  }
  
  function getResultCode() {
    return $this->resultcode;
  }
  
  function getBody() {
    $res =  ($this->module) ?
      array('sources', 'targets') :
      array('sources', 'targets', 'modules');
    return json_encode($res);
  }

  static function createList($myArgs, $prevArgs, $base, $module=null) {
    $c = count($myArgs);
    if ($c > 0) {
      $format = ($c > 1) ? $myArgs[1] : null;
      switch ($myArgs[0]) {
      case 'modules':
	return new ListModules($module);
      case 'targets':
	return new ListTargets($format, $module);
      case 'sources':
	return new ListSources($format, $module);
      }

      $list = new ListResults($module);
      $list->resultcode = 404;
      return $list;
    }
    else {
      return new ListResults($module);
    }
  }
}

class ListTargets extends ListResults {
  function __construct($sourceFormat, $module = null) {
    $this->sourceFormat = $sourceFormat;
    $this->module = $module;
  }

  function getBody() {
    $targets = ($this->module) ?
      $this->module::getTargets($this->sourceFormat):
      Converter::getAllTargets($this->sourceFormat);


    //Put selfconversion at the end
    if (array_key_exists($this->sourceFormat, $targets)) {
      $sourceTarget = $targets[$this->sourceFormat];
      unset($targets[$this->sourceFormat]);
      $targets[$this->sourceFormat]=$sourceTarget;
    }

    return json_encode($targets);
  }
}

class ListSources extends ListResults {
  function __construct($targetFormat, $module = null) {
    $this->targetFormat = $targetFormat;
    $this->module = $module;
  }

  function getBody() {
    $sources = ($this->module) ?
      $this->module::getSources($this->targetFormat):
      Converter::getAllSources($this->targetFormat);
    return json_encode($sources);
  }
}
class ListModules extends ListResults {
  function getBody() {
    $modules = ($this->module) ? array($this->module) : Converter::getAllModules();
    return json_encode($modules);
  }
}
?>
