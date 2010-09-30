<?php
class PXAdminTreeObjects extends PXAdminObjects {
	function __construct($format) {
		parent::__construct($format);
		$this->createSubWidget();
	}

	function loadObjects() {
		$this->tree = $this->db->getObjects($this->format, null, DB_SELECT_TREE);

		$this->db->filterByAccess('admin', $this->format, $this->tree);
		$this->count = sizeof($this->tree->leafs) - 1;
	}

	function getControlParent() {
		return $this->tree->getRoot()->id;
	}

	function createSubWidget() {
		$this->loadObjects();
		$this->widget = new PXAdminTree($this->format, $this->tree);
		$this->widget->setOpenedLeafs($this->request->getLeafStatus());
		$this->widget->loadDropdownValues($this->db);
	}

	function setSelected($selectedSid) {
		$this->widget->setSelected($selectedSid);
	}

	function appendControl($name, $template) {
		// pass
	}

	function has($id) {
		return isset($this->tree->leafs[$id]);
	}

	function get($id) {
		if(!$this->has($id)) {
			FatalError('Can\'t find '.$id);
		}

		return $this->tree->leafs[$id]->content;
	}
}
?>