<?php
abstract class Result {
  function setHeaders() {
    http_response_code($this->getResultCode());
    header('Content-Type: '. $this->getResultType());
  }
  function getResultCode() {
    return 200;
  }
  abstract protected function getResultType();
  abstract protected function getBody();
}
?>
