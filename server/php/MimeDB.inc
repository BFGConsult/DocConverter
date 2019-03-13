<?php
class MimeDB {
  static $instance = null;

  private function __construct() {
    $this->db =
      array_filter(
		   array(
			 'docbook' =>
			 array (
				'mime' => 'application/docbook+xml',
				'extension' => 'docbook',
				'pretty' => 'DocBook',
				),
			 'pdf' =>
			 array (
				'mime' => 'application/pdf',
				'extension' => 'pdf',
				'pretty' => 'Portable Document Format',
				),
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
			 'txt' =>
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

  static function getEntryFromMimeType($sourceFormat) {
    if (!self::$instance)
      self::$instance = new MimeDB();

    foreach (self::$instance->db as $key => $value) {
      if ($value['mime'] == $sourceFormat) {
	return $value;
      }
    }
    return null;
  }

  static function getEntryFromExtension($extension) {
    if (!self::$instance)
      self::$instance = new MimeDB();

    return (array_key_exists($extension, self::$instance->db)) ?
	    self::$instance->db[$extension] : null;
  }

}
?>