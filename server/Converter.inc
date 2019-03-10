<?php
include_once 'Result.inc';

abstract class Converter extends Result {
  protected $in_name;
  protected $in_mimetype;
  protected $out_mimetype;

  static $converters=array();

  abstract static function getTargets($sourceFormat);
  abstract static function getSources($sourceFormat);
  abstract static function createConverter($in_name, $in_type, $out_type);

  static function getAllSources($targetFormat = null) {
    $result=null;
    foreach (self::$converters as $converter) {
      //print $converter;
      if (!$result)
	$result = $converter::getSources($targetFormat);
      else
	array_push($result, $converter::getSources($targetFormat));
    }
    return $result;
  }

  static function getAllTargets($sourceFormat = null) {
    $result=null;
    foreach (self::$converters as $converter) {
      //print $converter;
      if (!$result)
	$result = $converter::getTargets($sourceFormat);
      else
	array_push($result, $converter::getTargets($sourceFormat));
    }
    return $result;
  }

  static function getAllModules() {
    return self::$converters;
  }

  static protected function registerConverter() {
    $class = get_called_class(); 
    //print $class;
    array_push(self::$converters, $class);
  }

  function __construct($in_name, $in_mimetype, $out_mimetype) {
    $this->in_name = $in_name;
    $this->in_mimetype = $in_mimetype;
    $this->out_mimetype = $out_mimetype;
  }

  function getInType() {
    return $this->in_type;
  }
  
  function getOutType() {
    return $this->out_type;
  }
  
  static function getConverter($in_name, $in_type, $out_type) {
    if (!is_string($out_type)) {
      throw new Exception("$out_type is not a string");
    }
    foreach (self::$converters as $converter) {
      $result=$converter::createConverter($in_name, $in_type, $out_type);
      if ($result)
	return $result;
    }
    return null;
  }
}

class IdentityConverter extends Converter {
  static $initialized=false;

  static function createConverter($in_name, $in_mimetype, $out_mimetype) {
    if ($in_mimetype==$out_mimetype) {
      return new IdentityConverter($in_name, $in_mimetype, $out_mimetype);
    }
    return null;
  }

  
  static function getTargets($sourceFormat) {
    return array();
  }
  static function getSources($sourceFormat) {
    return array();
  }

  
  static function init() {
    if (!self::$initialized)
      self::registerConverter();
    else
      self::$initialized=true;
  }
  
  function getResultType() {
    return $this->out_mimetype;
  }
  
  function getBody() {
    $result=file_get_contents($this->in_name);
    unlink($this->in_name);
    return $result;
  }  
}
IdentityConverter::init();


abstract class UnoConverter extends Converter {
  static $initialized=false;

  static protected abstract function getSourceFormats();
  static protected abstract function getTargetFormats();
  static protected abstract function getBothFormats();
  
  static function init() {
    if (!self::$initialized)
      self::registerConverter();
    else
      self::$initialized=true;
  }
  
  function getResultType() {
    return $this->out_mimetype;
  }

  static function flipArray($in_arr, $key) {
    $res = array();
    foreach ($in_arr as $akey => $avalue) {
      $res[$avalue[$key]]=$akey;
    }
    return $res;
  }
  
  function getBody() {
    $out_type = self::flipArray($this->getTargets(null),'mime')[$this->out_mimetype];
    
    $src_name=$this->in_name.".jau";
    $tgt_name=$this->in_name . ".${out_type}";
    rename($this->in_name, $src_name);

    $cmd="HOME=/var/www/home/ /usr/bin/unoconv -f ${out_type} -o $tgt_name $src_name 2>&1";
    $handle = popen($cmd, "r");
    $errors = fread($handle, 2096);
    pclose($handle);

    //Enable on production systems
    //unlink($src_name);

    if ( file_exists($tgt_name) ) {
      $result=file_get_contents($tgt_name);
      unlink($tgt_name);
    }
    else {
      $this->out_mimetype='text/plain';
      $result=$cmd;
      $result.="\n";
      $result.=$errors;
    }
    return $result;
  }  
}

class TextUnoConverter extends UnoConverter {

  static function createConverter($in_name, $in_mimetype, $out_mimetype) {
    //Very ugly hack
    if (!in_array($out_mimetype, array_flip(self::flipArray(self::getTargets(null),'mime'))))
      return null;

    //No input checking, we are saying we can convert anything to the give outputtypes
    return new TextUnoConverter($in_name, $in_mimetype, $out_mimetype);
  }

  static function getTargets($sourceFormat) {
    if (!$sourceFormat ||
	array_key_exists($sourceFormat, self::getSourceFormats()) ||
	array_key_exists($sourceFormat, self::getBothFormats())	
	) {
      return array_merge(self::getBothFormats(), self::getTargetFormats());
    }
    elseif (array_key_exists($sourceFormat, self::getTargetFormats())) {
	return self::getTargetFormats();
    }
    else
      return array();
  }

  static function getSources($targetFormat) {
    if (!$targetFormat ||
	array_key_exists($targetFormat, self::getBothFormats())	||
	array_key_exists($targetFormat, self::getTargetFormats())
	) {
      return array_merge(self::getSourceFormats(), self::getBothFormats());
    }
    elseif (array_key_exists($targetFormat, self::getSourceFormats())) {
	return self::getSourceFormats();
    }
    else
      return array();
  }

  static protected function getSourceFormats() {
    return
      array_filter(
		   array(
			 'docbook' =>
			 array (
				'mime' => 'application/docbook+xml',
				'extension' => 'docbook',
				'pretty' => 'DocBook',
				),
			 )
		   );
  }
  static protected function getTargetFormats() {
    return
      array_filter(
        array(
	      'pdf' =>
	      array (
		     'mime' => 'application/pdf',
		     'extension' => 'pdf',
		     'pretty' => 'Portable Document Format File',
		     ),
	      )
		   );
  }
  
  static protected function getBothFormats() {
      return
        array_filter(
          array(
	      'bib' => null,
	      'doc' => null,
	      'doc6' => null,
	      'doc95' => null,
	      'docx' =>
	      array (
		     'mime' => 'application/vnd.openxmlformats-officedocument.wordprocessingml.documentl',
		     'extension' => 'docx',
		     'pretty' => 'Microsoft Word (OpenXML)',
		     ),
	      'docx7' => null,
	      'fodt' => null,
	      'html' =>
	      array (
		     'mime' => 'text/html',
		     'extension' => 'html',
		     'pretty' => 'HTML',
		     ),
	      'latex' => null,
	      'mediawiki' => null,
	      'odt' =>
	      array (
		     'mime' => 'application/vnd.oasis.opendocument.text',
		     'extension' => 'odt',
		     'pretty' => 'OpenDocument text document',
		     ),
	      'ooxml' => null,
	      'ott' =>
	      array (
		     'mime' => 'application/vnd.oasis.opendocument.text-template',
		     'extension' => 'ott',
		     'pretty' => 'OpenDocument Document Template',
		     ),
	      'pdb' => null,
	      'psw' => null,
	      'rtf' =>
	      array (
		     'mime' => 'text/rtf',
		     'extension' => '.rtf',
		     'pretty' => 'Rich Text Format File',
		     ),
	      'sdw' => null,
	      'sdw4' => null,
	      'sdw3' => null,
	      'stw' => null,
	      'sxw' => null,
	      'text' => null,
	      'rtf' =>
	      array (
		     'mime' => 'text/plain',
		     'extension' => 'txt',
		     'pretty' => 'Plain Text File',
		     ),
	      'uot' => null,
	      'vor' => null,
	      'vor4' => null,
	      'vor3' => null,
	      'wps' => null,
	      'xhtml' => null,
	      )
		   );
  }
}

TextUnoConverter::init();
?>
