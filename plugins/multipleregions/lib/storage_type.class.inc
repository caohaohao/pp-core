<?php
/**
 * ������ URL ��������
 * � ���� ����� ��������� ��� VARCHAR(x), ��� � TEXT
 *
 * displayType URLTEXT
 * storageType regionurl
 */
class PXStorageTypeRegionUrl extends PXStorageTypeString {
	function getFromRequest($field, $object, &$param) {
		$k = $field->name;
		if (isset($object[$k]) && count($object[$k])) {
			$urls = array();

			foreach ($object[$k] as $url) {
				$urls[] = $url[0];
			}
			
			return implode(',', $urls);
		}
	}
}
?>