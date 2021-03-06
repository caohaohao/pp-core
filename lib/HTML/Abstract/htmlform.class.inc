<?php

define('IMAGE_DEFAULT', 'i/v.gif');
define('FLASH_DEFAULT', 'i/v.swf');
define('FLASH_DEFAULT_FULL', '/admin/i/v.swf');

class NLAbstractHTMLForm {
	static $disable = false;

	static
	function _DisabledStatus() {
		return self::$disable;
	}

	public static
	function setDisabledStatus($status) {
		self::$disable = !!$status;
	}

	static
	function BuildFormBegin($action='action.phtml', $method='POST', $enctype='multipart/form-data') {
		return '<form action="'.$action.'" method="'.$method.'" enctype="'.$enctype.'" class="edit">';
	}

	static
	function BuildFormEnd() {
		return '</form>';
	}

	static
	function BuildInputCheckBox($name, $on, $value=NULL, $id=NULL) {
		$checkbox = new PXInputCheckbox($name);

		$checkbox->setOn($on);
		$checkbox->setId($id);
		$checkbox->setValue($value);

		$checkbox->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $checkbox->html();
	}

	static
	function BuildCheckBox($name, $on, $value=NULL, $id=NULL) {
		return NLAbstractHTMLForm::BuildInputCheckBox($name, $on, $value, $id);
	}

	static
	function BuildSubmit($value, $name = null, $accessKey = null) {
		$submit = new PXControlSubmit($value, $name, $accessKey);
		$submit->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $submit->html();
	}

	function BuildButton($value, $onClickCode = '') {
		$button = new PXControlButton($value);
		$button->setClickCode($onClickCode);
		$button->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $button->html();
	}

	static
	function BuildReset($value) {
		$reset = new PXControlReset($value);
		$reset->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $reset->html();
	}

	static
	function BuildClose($value) {
		$close = new PXControlClose($value);
		return $close->html();
	}

	static
	function BuildHidden($name, $value) {
		$hidden = new PXInputHidden($name, $value);

		$hidden->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $hidden->html();
	}

	static
	function BuildText($name, $value, $height, $maxlength=null, $wrap = "soft") {
		if ($height > 0) {
			$input = new PXInputTextarea($name, $value, $height, $maxlength, $wrap);
		} else {
			$input = new PXInputInput($name, $value, $maxlength);
		}

		$input->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $input->html();
	}

	static
	function BuildPassword($name, $value, $canDelete=true) {
		$password = new PXInputPassword($name, $value);
		$password->canDelete($canDelete);
		$password->setDisabled(NLAbstractHTMLForm::_DisabledStatus());

		return $password->html();
	}

	static
	function BuildColor($name, $value) {
		$color = new PXInputColor($name, $value);
		$color->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $color->html();
	}

	static
	function BuildRichEdit($name, $value, $height, $redactor = null) {
		$richedit = new PXInputRichEdit($name, $value, $height);
		$richedit->setFavoriteEditor($redactor);
		$richedit->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $richedit->html();
	}

	static
	function BuildDateInline($name, $value) {
		$date = new PXInputDate($name, $value);

		$date->setDisabled(NLAbstractHTMLForm::_DisabledStatus());

		return $date->html();
	}

	static
	function BuildDateTimeInline($name, $value) {
		$time = new PXInputTime($name, $value);
		$date = new PXInputDate($name, $value);

		$time->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		$date->setDisabled(NLAbstractHTMLForm::_DisabledStatus());

		return $time->html().' '.$date->html();
	}

	static
	function BuildDateTime($name, $value, $showTime, $showDate, $useCalendar, $valueNotNull = false) {
		$timeCls = $showTime ? 'PXInputTime' : 'PXInputTimeHidden';
		$dateCls = $showDate ? ($useCalendar ? 'PXInputDateCalendar' : 'PXInputDate') : 'PXInputDateHidden';

		$time = new $timeCls($name, $value, $valueNotNull);
		$date = new $dateCls($name, $value, $valueNotNull);

		$time->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		$date->setDisabled(NLAbstractHTMLForm::_DisabledStatus());

		return $time->html().' '.$date->html();
	}

	static
	function BuildMultimedia($name, $value, $type='image', $entry=NULL) {
		switch($type) {
			case 'image':
				$input = new PXInputImage($name, $value, $entry);
				break;

			case 'flash':
				$input = new PXInputFlash($name, $value, $entry);
				break;

			case 'file':
				$input = new PXInputFile($name, $value, $entry);
				break;

			default:
				FatalError('Undefined input type '.$type);
		}

		$input->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $input->html();
	}

	static
	function BuildLinkToFile($name, $value) {
		$link = new PXInputLinkToFile($name, $value);
		$link->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $link->html();
	}

	static
	function BuildImage($name, $value) {
		$image = new PXInputImage($name, $value);
		$image->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $image->html();
	}

	static
	function BuildFlash($name, $value) {
		$flash = new PXInputFlash($name, $value);
		$flash->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $flash->html();
	}

	static
	function BuildFile($name, $value) {
		$file = new PXInputFile($name, $value);
		$file->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $file->html();
	}

	static
	function BuildDropDown($name, $value, $values, $multiple=false, $size=null, $disabled=false) {
		$dropdown = new PXInputDropDown($name, $value, $values);
		$dropdown->setDisabled($disabled||NLAbstractHTMLForm::_DisabledStatus());

		$dropdown->setMultiple($multiple);

		if ($size) {
			$dropdown->setSize((int) $size);
		}


		return $dropdown->html();
	}

	static
	function BuildParentDropDown($name, $value, $values) {
		$parentdropdown = new PXInputParentDropDown($name, $value, $values);
		$parentdropdown->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $parentdropdown->html();
	}

	static
	function BuildTable($name, $value, $cols, $height, $header = array()) {
		$colsWidth = array();

		for ($i=0; $i<$cols; $i++) {
			$colsWidth[] = 1;
		}

		return NLAbstractHTMLForm::BuildTableByCols($name, $value, $colsWidth, $height, $header);
	}

	static
	function BuildTableByCols($name, $value, $cols, $height, $header = array()) {
		$table = new PXInputTable($name, $value, $cols, $height, $header);
		$table->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $table->html();
	}

	static
	function BuildIPAddr($name, $value) {
		$ip = new PXInputIPAddr($name, $value);
		$ip->setDisabled(NLAbstractHTMLForm::_DisabledStatus());
		return $ip->html();
	}

	static
	function BuildUserType($name, $value, $color, &$field) {
		return $field->displayType->name;
	}
}
