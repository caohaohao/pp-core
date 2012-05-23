<?php
/**
 * "�����������" ���� - ��������� ��� ��������� ����� �������
 * ����� �� ��������, �������� ����������� � ��������������� ������� (����� ��������� ���� ��������� �������-����������)
 *
 * displayType - various (ex. STATIC)
 * storageType virtual
 * storageTypeArgs - aggregator_name|aggregator_arg1|aggregator_arg2|...
 * ������ ���������� CTR ��� adbanner datatype:
 * <attribute name="ctr" description="CTR" storagetype="aggr|div_percent|clicks|shows" displaytype="STATIC" defaultvalue="0" />
 */
class PXStorageTypeAggr extends PXStorageTypeVirtual {

	public function normalize($field, $object, &$param) {

		$args = $field->storageTypeArgs;
		$res  = parent::normalize($field, $object, $param);
		
		if (empty($args)) {
			return $res;
		}

		$cb = array_shift($args);
		array_unshift($args, $object);

		if (is_callable($_ = array($this, "aggr_" . $cb))) {
			$res = call_user_func_array($_, $args);
		} else if (is_callable($cb)) {
			$res = call_user_func_array($cb, $args); // for local project custom aggregators
		}

		return $res;
	}
	
	/**
	 * Div aggregator
	 * @param type $a first arg: sum
	 * @param type $b the second arg: qty
	 * @param type $c the third arg: precision
	 * @return float
	 */
	protected function aggr_div($object, $a, $b, $c = 2) {
		return (empty($object[$a]) || empty($object[$b])) ? 0.0 : round($object[$a]/$object[$b], (int)$c);
	}
	
	/**
	 * Div percentage aggregator
	 * @param type $a first arg: dividend
	 * @param type $b the second arg: divider
	 * @param type $c the third arg: precision
	 * @return float
	 */
	protected function aggr_div_percent($object, $a, $b, $c = 2) {
		return $this->aggr_div($object, $a, $b, (int)$c + 2) * 100 . '%';
	}
}

?>