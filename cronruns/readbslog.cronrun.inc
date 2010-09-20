<?php
class PXCronRunReadBSLog extends PXAbstractCronRun {
	var $DATE_FORMAT   = 'd.m.Y H:00:00';              // date format here [d-m-Y H:i:s] german time format

	var $RELOAD_FILE   = '/site/var/ad/in/reload'; // ���� �� �������� �������� ������. ������������ BASEPATH
	var $BS_OUT_DIR    = '/site/var/ad/out/';      // ������� � bs log files (�������). ������������ BASEPATH
	var $LOGS_DIR      = '/site/var/ad/logs/';     // ������� � ������������ ������ ������� (�����). ������������ BASEPATH

	function PXCronRunReadBSLog() {
		$this->name = '��������� ����� ��������� �������';

		$this->result = array();
		$this->files  = array(); // ������ � ������� ����������

		$this->audit  = array(); // ���������� audit letters
		$this->stats  = array(); // ���������� �������/������

		$this->total  = array(
			'clicks' => 0,
			'shows'  => 0,
			'audit'  => 0
		);

		$this->timeMin = 0;
		$this->timeMax = 0;
	}

	function Run(&$app, &$db, &$tree, $mTime, $mRule) {
		$this->db =& $db;

		Label('Starting...');

		$message = $this->touchReload();
		if(is_string($message)) {
			return array('status' => -1, 'note' => $message);
		}

		$message = $this->filesList();
		if(is_string($message)) {
			return array('status' => -2, 'note' => $message);
		}

		$message = $this->processing();
		if(is_string($message)) {
			return array('status' => -3, 'note' => $message);
		}

		$this->insertBannerStats();
		$this->insertLettersAudit();

		return array('status' => 0, 'note' => '�������! (clicks/shows/audit: '.$this->total['clicks'].'/'.$this->total['shows'].'/'.$this->total['audit'].')');
	}

	function touchReload() {
		if (!@touch(BASEPATH.$this->RELOAD_FILE)) {
			$message = 'Cannot touch file: '.$this->RELOAD_FILE;
			Label($message);
			return $message;
		}

		sleep(1);
	}

	function filesList() {
		Label('Get files list');
		$handle = @opendir(BASEPATH.$this->BS_OUT_DIR);
		if (!$handle) {
			$message = 'Cannot open dir: '.BASEPATH.$this->BS_OUT_DIR;
			Label($message);
			return $message;
		}

		while (false !== ($file = readdir($handle))) {
			WorkProgress();

			if (preg_match('/^log\.(\d+)/', $file, $match)) {
				$this->files[$match[1]] = BASEPATH.$this->BS_OUT_DIR.$file;
			}
		}
		WorkProgress(true);

		closedir($handle);
		ksort($this->files);
	}

	function processing() {
		if(!sizeof($this->files)) {
			$message = 'Nothing to do';
			Label($message);
			return $message;
		}

		Label('Processing '.sizeof($this->files).' files...');

		foreach ($this->files as $file) {
			$this->_parseFile($file);

			unlink($file);
		}
		WorkProgress(true);
	}

	function _parseFile($file) {
		Label('Processing file: '.$file);

		$t = @stat($file);

		if (!$t['size']) {
			return;
		}

		$fileContents = file_get_contents($file);
		$fileStrings  = explode("\n", $fileContents);

		foreach ($fileStrings as $s) {
			WorkProgress();
			$this->_parseLine($s);
		}
	}

	function _parseLine($s) {
		if (!strlen($s)) {
			return;
		}

		$temp     = explode(" ", $s);
		$adplace  = $temp[3];
		$adbanner = $temp[4];
		$time     = date($this->DATE_FORMAT, $temp[0]);
		$action   = ($temp['2'] == 'clk') ? 'clk' : 'show';

		//calculate minimal and maximal time from new logs
		if ($this->timeMin != 0) {
			if ($this->timeMin >= $temp[0]) {
				$this->timeMin = $temp[0];
			}

		} else {
			$this->timeMin = $temp[0];
		}

		if ($this->timeMax != 0)  {
			if ($this->timeMax <= $temp[0]) {
				$this->timeMax = $temp[0];
			}

		} else {
			$this->timeMax = $temp[0];
		}

		//log it
		$logFileName = date('Y-m-d', $temp[0]).'.log';
		$this->_log($s."\n", $logFileName);

		//put to array ,-)
		switch (sizeof($temp)) {
			case 6: //v1 logs
			case 7:
				$this->_arrayInc($this->stats[$time][$adplace][$adbanner][$action]);
				break;

			case 8: //v2 logs
				if ($temp[7] === "audit") {
					$this->_arrayInc($this->audit[$adbanner]); //audit action here
				} else {
					$this->_arrayInc($this->stats[$time][$adplace][$adbanner][$action]);
				}
				break;

			default:
				//Label('Broken string found in file: [' . $file . '] at line: ['.$i.'] skipping...');
				break;
		}
	}

	function insertLettersAudit() {
		Label('Update audit letters stats...');

		//update audit letters stats
		foreach ($this->audit as $id => $shows) {
			WorkProgress();
			$this->db->query("UPDATE newsletter SET shows = shows + {$shows} WHERE id = {$id}");

			$this->total['audit'] += $shows;
		}
		WorkProgress(TRUE);
	}

	function insertBannerStats() {
		Label('Insert banners stats...');

		$db =& $this->db;

		$timeMin = date($this->DATE_FORMAT, $this->timeMin);
		$timeMax = date($this->DATE_FORMAT, $this->timeMax);

		$sql = "SELECT ts, adplace, adbanner FROM adstat WHERE ts BETWEEN '".$timeMin."' AND '".$timeMax."'";
		$rowsToUpdate = $db->query($sql);

		if(!is_array($rowsToUpdate)) {
			$rowsToUpdate = array();
		}

		Label('rowsToUpdate - '.sizeof($rowsToUpdate));

		//insert new banner stats into db
		foreach ($this->stats as $timestamp => $timestampData) {
			WorkProgress();

			foreach ($timestampData as $place => $placeData) {
				WorkProgress();

				foreach ($placeData as $banner => $actions) {
					WorkProgress();

					$show = isset($actions['show']) ? $actions['show'] : 0;
					$clk  = isset($actions['clk'])  ? $actions['clk']  : 0;

					// update existing record
					if (in_array(array('ts'=>$timestamp, 'adplace'=>$place, 'adbanner'=>$banner), $rowsToUpdate)) {
						$sql = "UPDATE adstat SET show = show + '{$show}', click = click + '{$clk}' ";
						$sql.= "WHERE ts = '{$timestamp}' AND adplace = '{$place}' and adbanner='{$banner}'";
						$act = "U";

					// or insert new row
					} else {
						$sql = "INSERT INTO adstat (adplace, adbanner, ts, show, click) ";
						$sql.= "VALUES ('{$place}','{$banner}', '{$timestamp}', '{$show}', '{$clk}');";
						$act = "I";
					}

					Label ($timestamp ."\t". $place ."\t". $banner ."\t". $show ."\t". $clk ."\t[".$act."]");
					$db->query($sql);

					// and update adbanner table...
					if ($show > 0) {
						$db->query("UPDATE adbanner SET shows  = shows  + '{$show}' WHERE id = {$banner}");
						$this->total['shows'] += $show;
					}

					if ($clk > 0) {
						$db->query("UPDATE adbanner SET clicks = clicks + '{$clk}'  WHERE id = {$banner}");
						$this->total['clicks'] += $clk;
					}

				}
			}
		}
		WorkProgress(TRUE);
	}

	function _arrayInc(&$arr) {
		if (isset($arr)) {
			$arr++;
		} else {
			$arr = 1;
		}
	}

	function _log($str, $logNameTemp) {
		static $logHandler = null;
		static $logName    = '';

		if ($logName != $logNameTemp) {
			if ($logName != '') {
				fclose($logHandler);
			}

			$logName    = $logNameTemp;
			$logHandler = fopen (BASEPATH.$this->LOGS_DIR.$logName, 'a');

			Label('creating file '.$logName);
		}

		fwrite($logHandler, $str);
	}
}

/*

�������� �������� �����

version1:

	formal:
	[unix timestamp] [ip address] [action] [placeid] [bannerid] [apache uid]

version2:

	formal:
	[unix timestamp] [ip address] [action] [placeid] [bannerid] [apache uid] [type] [template]

	[unix timestamp] - ������� unix time �� ������ �����
	[ip address]     - IP ����� � ���� nnn.nnn.nnn.nnn
	[action]         - ["show", "clk"] ����� �������, ���� �� �������
	[placeid]        - id ����� ������ � ��
	[bannerid]       - id ������� � ��
	[apache uid]     - ���������� id �����, �� ����� �� ��� ,-)
	[type]           - ��� �������, ��� ��� ������ "NNNxNNN"
	[template]       - ������ ������ ������� [gif, flash, audit... ?]

	example:
	1097241447 192.168.1.21 show 281470 281471 5113442241610AF8000007BA03035D02 123x123 gif

*/

?>