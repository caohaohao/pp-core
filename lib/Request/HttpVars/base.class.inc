<?php

require_once PPLIBPATH . 'Common/functions.array.inc';

class NLHTTPVarsData {
	var $raw;
	var $clientCharset;
	var $_needToStripTags;

	function __construct($needToStripTags = false, $needToConvert=true) {
		$this->raw              = array();
		$this->clientCharset    = isset($_SERVER['CHARSET']) ? strtolower($_SERVER['CHARSET']{0}) : CHARSET_UTF8;
		$this->_needToStripTags = $needToStripTags;
	}

	// Раскодирование JavaScript escape();
	// private
	function _unEscape(&$data) {
		if (is_array($data)) {
			foreach ($data as $k=>$v) {
				$this->_unEscape($data[$k]);
			}

		} else {
			if(preg_match('/^(%u\d{4})+$/'.REGEX_MOD, $data)) {
				$data = utf8_urldecode($data);
			}
		}
	}

	function _NormalizeData() {
		$this->_unEscape($this->raw);
		$this->_Normalize($this->raw);
	}

	function _Normalize(&$data) {
		if (is_array($data)) {
			foreach ($data as $k=>$v) {
				$this->_Normalize($data[$k]);
			}

		} else {
			if (get_magic_quotes_gpc()) {
				$data = stripslashes($data);
			}

			if ($this->_needToStripTags) {
				$data = strip_tags($data);
			}
		}
	}

	function _GetVar($varName) {
		return isset($this->raw[$varName]) ? $this->raw[$varName] : NULL;
	}

	function _isSetVar($varName) {
		return array_key_exists($varName, $this->raw);
	}

	function _SetVar($varName, $value) {
		$this->raw[$varName] = $value;
	}

	function _GetNumericVar($varName) {
		if (!isset($this->raw[$varName])) {
			return NULL;
		}
		if (!is_numeric($this->raw[$varName])) {
			return NULL;
		}
		return $this->raw[$varName];
	}

	function _GetArrayVar($varName, $killEmptyRows = true) {
		if (!isset($this->raw[$varName])) {
			return array();
		}
		if (!is_array($this->raw[$varName])) {
			return array();
		}

		$temp = $this->raw[$varName];

		static $callback;

		if(!isset($callback)) {
			$callback = create_function('&$arrayItself, $key, $opts', <<<DO
				if(empty(\$arrayItself[\$key]) && !in_array(\$arrayItself[\$key], array(0, "0"), true) && \$opts['killEmptyRows']) unset(\$arrayItself[\$key]);
DO
			);
		}
		NestedArrayWalk($temp, $callback, array('killEmptyRows' => $killEmptyRows));

		return $temp;
	}

	function _GetCheckboxVar($varName) {
		if (!isset($this->raw[$varName])) {
			return false;
		}

		return true;
	}

	function _GetTimeStampVar($varName) {
		if (!isset($this->raw[$varName])) {
			return date("d-n-Y G:i:s");
		} elseif($this->raw[$varName]['year'] == '' && $this->raw[$varName]['month'] == '' && $this->raw[$varName]['day'] == '') {
			return NULL;
		} else {
			$r = $this->raw[$varName];

			$pattern = array("year", "month", "day", "hour", "minute", "second");
			$diff = array_diff($pattern, array_keys($r));

			if(!empty($diff))
				return null;

			return call_user_func_array("sprintf", array("pattern"=>"%02d.%02d.%04d %02d:%02d:%02d", $r['day'], $r['month'], $r['year'], $r['hour'],
				$r['minute'], $r['second']));
		}
	}

	function _GetDateVar($varName) {
		if (!isset($this->raw[$varName])) {
			return date('j-n-Y');
		} else {
			$r = $this->raw[$varName];

			$reverse = array_flip($r);
			if(isset($reverse['']))
				return null;

			$pattern = array("year", "month", "day");
			$diff = array_diff($pattern, array_keys($r));

			if(!empty($diff))
				return null;

			return call_user_func_array("sprintf", array("pattern"=>"%02d.%02d.%04d",
				$r['day'], $r['month'], $r['year']));
		}
	}

	function _GetIPAddrVar($varName) {
		if (isset($this->raw[$varName])) {
			$val = implode(".", $this->raw[$varName]);
			return filter_var($val, FILTER_VALIDATE_IP) ? ip2long($val) :  0;
		}
	}

	function _IsSet($varName) {
		return isset($this->raw[$varName]);
	}

	function GetAll() {
		$retArray = array();
		foreach ($this->raw as $k=>$v) {
			if (is_array($v)) {
				$retArray[$k] = $this->_GetArrayVar($k);
			} else {
				$retArray[$k] = $this->_GetVar($k);
			}
		}
		return $retArray;
	}

}
