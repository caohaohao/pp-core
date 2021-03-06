<?php

class PXResponse {
	var $STATUS_OK                      = '200 OK';
	var $STATUS_MOVED                   = '301 Moved Permanently';
	var $STATUS_FOUND                   = '302 Found';
	var $STATUS_NOT_MODIFIED            = '304 Not Modified';
	var $STATUS_NOT_FOUND               = '404 Not Found';
	var $STATUS_FORBIDDEN               = '403 Forbidden';
	var $STATUS_INTERNAL_ERROR          = '500 Internal Server Error';
	var $STATUS_NOT_IMPLEMENTED         = '501 Not Implemented';
	var $STATUS_TEMPORARILY_UNAVAILABLE = '503 Service Temporarily Unavailable';

	var $headers = array();
	var $status;
	var $contentType = array();

	public function __construct() {
		$this->status = $this->STATUS_OK;

		try {
			$app       = PXRegistry::getApp(); // APP may not be initialized yet and throws exception
			$charset   = $app->GetProperty('OUTPUT_CHARSET', DEFAULT_CHARSET);
			$cacheTime = $app->GetProperty('PP_RESPONSE_CACHE_EXPIRATION', 3600);

		} catch (Exception $e) {
			$charset   = DEFAULT_CHARSET;
			$cacheTime = 3600;
		}

		$this->cache($cacheTime);
		$this->setContentType('text/html', $charset);
	}

	static function &getInstance() {
		static $instance;

		if (is_null($instance)) {
			$instance = new PXResponse();
		}

		return $instance;
	}

	function redirect($url, $cacheTimeOut = null) {
		if (ini_get('display_errors') && PXErrorReporter::hasErrors(true)) {
			exit();
		}

		// sanitize url
		$url = strtr($url, array(
				' '   => '%20',
				"\r"  => '',
				"\n"  => '',
				"\t"  => '%20',
				'%0D' => '',
				'%0A' => '',
				'%09' => '%20',
				'%00' => ''
			)
		);

		// this is non-RFC compliant mode
		// @see: https://github.com/guzzle/guzzle3/blob/b3f5050cb6270c7a728a0b74ac2de50a262b3e02/src/Guzzle/Http/RedirectPlugin.php#L128
		// for explanations
		$this->status = $this->STATUS_MOVED;

		$this->addHeader('Location', $url);

		if (!is_null($cacheTimeOut)) {
			$this->cache($cacheTimeOut);
		} else {
			$this->dontCache();
		}

		$this->send();

		exit();
	}

	function addHeader($name, $value) {
		$this->headers[$name] = $value;
	}

	function removeHeader($name) {
		unset($this->headers[$name]);
	}

	function setOk() {
		$this->status = $this->STATUS_OK;
	}

	function notFound() {
		$this->status = $this->STATUS_NOT_FOUND;
	}

	function notModified() {
		$this->status = $this->STATUS_NOT_MODIFIED;
	}

	function forbidden() {
		$this->status = $this->STATUS_FORBIDDEN;
	}

	// Do nothing with cache for 500s answers at Proxima side
	function internalError() {
		$this->status = $this->STATUS_INTERNAL_ERROR;
		$this->noCache();
	}

	function notImplemented() {
		$this->status = $this->STATUS_NOT_IMPLEMENTED;
		$this->noCache();
	}

	function unavailable() {
		$this->status = $this->STATUS_TEMPORARILY_UNAVAILABLE;
		$this->noCache();
	}

	function isError() {
		return intval($this->status) >= 400;
	}

	// Cache management
	function dontCache() {
		$this->addHeader('X-Accel-Expires',  0);
		$this->addHeader('Cache-Control', 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0');
		$this->addHeader('Expires', DateUnixToGMT());
	}

	function noCache() { // not sure about caching behaviour, make upstream server responsible for this
		$this->removeHeader('X-Accel-Expires');
		$this->removeHeader('Cache-Control');
		$this->removeHeader('Expires');
	}

	function cache($timeOut = 3600, $xae = null) {
		$this->addHeader('X-Accel-Expires', isset($xae) ? (int)$xae : (int)$timeOut);
		$this->addHeader('Cache-Control', sprintf('private, max-age=%d', (int)$timeOut));
		$this->addHeader('Expires', DateUnixToGMT(time()+(int)$timeOut));
	}

	function setLength($length) {
		$this->addHeader('Content-Length', $length);
	}

	function setCharset($charset) {
		$this->setContentType($this->contentType['type'], $charset);
	}

	function setLastModified($timestamp){
		$this->addHeader('Last-Modified', DateUnixToGMT($timestamp));
	}

	function setContentType($contentType, $charset=null) {
		$this->contentType = array(
			'type'    => $contentType,
			'charset' => NULL
		);

		if (mb_strlen($charset)) {
			$this->contentType['charset'] = $charset;
			$contentType .= ';charset='.$charset;
		}

		$this->addHeader('Content-Type', $contentType);
	}

	function downloadFile($filename, $contentType = null, $dispositionType = 'attachment', $charset=null) {
		if (mb_strlen($contentType)) {
			$this->setContentType($contentType, $charset);
		}
		$this->addHeader('Content-Disposition', $dispositionType . '; filename="'. $filename .'"');
	}

	function send($content = null, $http_version_1_1 = true) {
		if (!headers_sent()) {
			// FIXME: we really need HTTP/1.1 ?
			// to fix troubles with Transfer-Encoding: chunked
			// (Apache uses it when Content-Length header missed for 1.1)
			// you may use Nginx directive "chunked_transfer_encoding off;"
			// in the http{} or location{} config scopes
			header(sprintf('HTTP/1.%d %s', (int)$http_version_1_1, $this->status));

			if (mb_strlen($content)) {
				// NOTICE: no mb_ here
				$this->setLength(strlen($content));
			}

			$this->_sendHeaders();
		}

		if (mb_strlen($content)) {
			echo $content;
		}
	}

	function _sendHeaders() {
		$sentHeaders = function_exists('apache_response_headers') ? apache_response_headers() : array();
		$notSent = !is_array($sentHeaders);

		foreach($this->headers  as $header => $value) {
			if($notSent || !isset($sentHeaders[$header])) {
				header($header.': '.$value);
			}
		}

		if (function_exists('sendheaders')) {
			sendheaders(); // TODO: remove
		}
	}

	/**
	 * Set cookie
	 *
	 * (Don't send cookie, if headers already sent)
	 *
	 * @param string $name
	 * @param mixed $value
	 * @param integer $expire
	 * @param string $domain
	 * @param string $path
	 * @param boolean $secure
	 * @param boolean $httponly
	 * @return boolean
	 */
	function SetCookie($name, $value, $expire = NULL, $domain = NULL, $path = '/', $secure = false, $httponly = false) {
		if (headers_sent()) {
			return false;
		}
		if (is_array($value) || is_object($value)) {
			$value = serialize($value);
		}

		if (!is_numeric($expire)) {
			$expire = 0;
		}

		return setcookie($name, $value, $expire, $path, $domain, $secure, $httponly);
	}
}

?>
