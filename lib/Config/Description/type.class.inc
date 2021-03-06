<?php
/**
* Класс - хранилище.
* Стоится по etc/datatype.xml /model/datatypes/datatype
*
* @subpackage Application
*/
class PXTypeDescription {
	var $id            = null;
	var $title         = null;
	var $parent        = null;
	var $childs        = [];
	var $references    = [];
	var $order         = null;

	/** @var PXFieldDescription[] */
	public $fields        = [];

	var $groups        = array('' => array('name' => null, 'view' => null)); //TODO|FIXME: need standalone group description ?
	var $fieldGroups   = [];
	var $sTypes        = [];
	var $dTypes        = [];
	var $struct        = 'plain';
	var $byDefault     = null;

	function parentType() {
		return PXRegistry::getTypes($this->parent);
	}

	/**
	 * Возвращает массив типов потомков для переданного формата
	 */
	function childTypes() {
		$cTypes = array();

		if(count($this->childs) > 0){
			foreach ($this->childs as $v) {
				if(!$this->issetType($v)) {
					continue;
				}

				if ($v === $this->id){
					continue;
				}

				$cTypes[$v] =& $this->getType($v);
			}
		}

		return $cTypes;
	}

	function allowedChilds($object) {
		$ac = [];
		if (is_object($object)) {
			$ac = (isset($object->content[OBJ_FIELD_CHILDREN]))
				? $object->content[OBJ_FIELD_CHILDREN]
				: [];
		}

		if (is_array($object)) {
			$ac = (isset($object[OBJ_FIELD_CHILDREN]))
				? $object[OBJ_FIELD_CHILDREN]
				: [];
		}

		foreach ($ac as $k=>$v) {
			if (!$this->issetType($k)) {
				unset($ac[$k]);
			}
		}

		return $ac;
	}

	function allowedChildTypes(&$object) {
		$allowed = $this->allowedChilds($object);

		foreach($allowed as $format=>$v) {
			$allowed[$format] = $this->getType($format);
		}

		return $allowed;
	}

	function issetType($typeName) {
		$allTypes = PXRegistry::getTypes();
		return isset($allTypes[$typeName]) && is_object($allTypes[$typeName]);
	}

	function &getType($typeName) {
		$allTypes = PXRegistry::getTypes();
		return $allTypes[$typeName];
	}

	// static parser factory
	static function fillAppTypes($domDatatypes, &$app) {

		if (!is_array($domDatatypes) || count($domDatatypes) == 0) {
			FatalError("Не определена объектная модель проекта");
		}

		foreach ($domDatatypes as $domDatatype) {
			$type  = new static();

			$type->parseTypeAttributes($domDatatype->attributes());

			$groupNodes = $domDatatype->xpath("group");
			foreach ($groupNodes as $groupNode) {
				if (!$groupNode->isXmlNode()) {
					continue;
				}
				$name = $view = null;
				foreach($groupNode->attributes() as $attr){
					switch ($attr->name) {
						case 'name':
							$name = pp_simplexml_decode_string($attr->value);
							break;
						case 'view':
							$view = pp_simplexml_decode_string($attr->value);
							break;
					}
				}
				$type->groups[$name] = array('name' => $name, 'view' => $view);
			}

			$fieldsNodes = $domDatatype->xpath('descendant-or-self::attribute');

			foreach ($fieldsNodes as $fieldNode) {
				if (!$fieldNode->isXmlNode()) {
					continue;
				}

				$field = new PXFieldDescription($fieldNode, $app, $type);

				if(is_null($field->storageType)) {
					$field->setStorageType('string');
				}

				if(is_null($field->displayType)) {
					$field->setDisplayType('TEXT');
				}

				if(is_null($field->listed)) {
					$field->listed = (bool) ($field->name === 'title');
				}

				if(is_null($field->defaultValue)) {
					$field->getDefaultValueNode($fieldNode);
				}

				$field->getGroupName($fieldNode);
				$field->getComments($fieldNode);

				$type->addField($field);
				$type->assignToGroup($field);
			}

			$app->types[$type->id] = $type;
		}

		static::findParents($app->types);
	}

	static function findParents(&$types) {
		foreach ($types as $k=>$v) {
			if (!empty($v->parent)) {
				if (isset($types[$v->parent])) {
					$types[$v->parent]->childs[] = $k;

				} else {
					FatalError("ОШИБКА МОДЕЛИ: {$v->id} не может быть дочерним по отношению к {$v->parent}, описание {$v->parent} отсутствует");
				}
			}
		}
	}

	function addField(&$field) {
		$name = $field->name;
		$this->fields[$name] = $field;
	}

	/**
	 * @param string $name
	 * @return bool
	 */
	function hasField($name) {
		return isset($this->fields[$name]);
	}

	function assignToGroup($field) {
		static $groupId = 0;
		static $fillingGroup;
		if($fillingGroup !== $field->groupName){
			$fillingGroup = $field->groupName;
			$groupId++;
		}
		$this->fieldGroups[$groupId][$field->groupName][$field->name] = $field;
	}


	function parseTypeAttributes($attrs) {
		foreach ($attrs as $attr) {
			switch ($attr->name) {
				case 'name'          : $this->id            = $attr->value; break;
				case 'description'   : $this->title         = pp_simplexml_decode_string($attr->value); break;
				case 'orderby'       : $this->order         = $attr->value; break;
				case 'parent'        : $this->parent        = $attr->value; break;
				case 'bydefault'     : $this->byDefault     = self::_mapByDefaults($attr->value); break;
				case 'struct'        : $this->struct        = $attr->value; break;

				default              :
					throw new Exception('Unknown attr name:' . $attr->name);
			}
		}

		if (!isset($this->order)) {
			$this->order = OBJ_FIELD_ORDER;
		}
	}

	/**
	* Преобразует 'all', 'selected', 'none' в числовые значения.
	* protected -> public, because used in PXAdminForm
	*
	* @see PP_CHILDREN_FETCH_ALL, PP_CHILDREN_FETCH_SELECTED, PP_CHILDREN_FETCH_NONE
	* @access public
	* @param String $string
	* @return Integer
	*/
	public static function _mapByDefaults($string) {
		switch($string) {
			case 'all'     : return PP_CHILDREN_FETCH_ALL;
			case 'selected': return PP_CHILDREN_FETCH_SELECTED;
			case 'paged'   : return PP_CHILDREN_FETCH_PAGED;
			case 'none'    : return PP_CHILDREN_FETCH_NONE;
			default        : return NULL;
		}
	}

}
