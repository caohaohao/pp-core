<?php
/**
 * Строка
 * В базе может храниться как VARCHAR(x), так и TEXT
 *
 * displayType HIDDEN, TEXT, RICHEDIT, DROPDOWN, COLOR, STATIC
 * storageType string
 */
class PXStorageTypeString extends PXStorageType {
	function objectsSortOrder($field, $object, &$param = NULL) { /* E_STRICT */
		return $this->objectsSortOrderString($field, $object, $param);
	}

	function getFromRequest($field, $object, &$param) {
		return (string)$object[$field->name];
	}

	function normalizeObjectAttribute($field, $object, &$param) {
		return $this->normalizeObjectAttributeString($field, $object, $param);
	}

	function searchLikeQuery($field, $object, &$param) {
		return $param['formatId'].'.'.$field->name.PXRegistry::getDb()->LIKE($param['word'], P_LEFT | P_RIGHT).'OR ';
	}
	
	function filterable(){
		return true;
	}
}
?>