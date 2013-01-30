<?php
require_once 'ajaxTreeBranch.class.inc';

class PXAdminAjaxTree extends PXAdminTree {
	protected $currentLeaf;
	/**
	 * ����������� ������ PXAdminAjaxTree.
	 * �������������� ���� ������. ����������� ��� ���������� ������.
	 */
	public function __construct(&$datatype, &$tree, $currentLeafId = null) {
		parent::__construct($datatype, $tree);
		$this->currentLeaf = $currentLeafId;
	}

	public function html() {
		return isset($this->currentLeaf) ? $this->branchHtml() : $this->treeHtml();
	}
	
	protected function branchHtml(){
		return isset($this->tree->leafs[$this->currentLeaf])
			? PXAdminAjaxTreeBranch::childrenHTML($this->tree->leafs[$this->currentLeaf]->children, $this)
			: '';
	}
	
	protected function treeHtml(){
		$caption = $this->htmlCaption();

		if(sizeof($this->tree->leafs[0]->children) > 0) {
			$data = $this->showChildLinks ? ' data-cl="'.$this->childLinkVarName.'"' : '';
			return '<div class="tree"' . $data . '>' . $caption . PXAdminAjaxTreeBranch::childrenHTML($this->tree->leafs[0]->children, $this) . '</div>';
		} else {
			return '<div class="tree">' . $caption . '<div class="empty">&#8212;</div></div>';
		}
	}
	
	public /* override */ function isOpenLeaf($id) {
		return in_array($this->datatype->id . $id, $this->leafsToOpen);
	}
	
	public /* override */ function setSelected($selectedSid) {
		$this->selectedSid = $selectedSid;
	
		// ��������� � ������ "����������������" ������� ���� ������� ����������
		$tmpId = $selectedSid;
		while (isset($this->tree->leafs[$tmpId]->parent)) {
			$tmpId = $this->tree->leafs[$tmpId]->parent;
			$this->leafsToOpen[] = $this->datatype->id . $tmpId;
		}
	
	}
	
	public function isLastLeaf($id) {
		if(!isset($this->tree->leafs[$id])){
			return false;
		}
		$leaf = $this->tree->leafs[$id];
		$pid = $leafs->parent;
		if(!isset($this->tree->leafs[$pid])){
			return false;
		}
		$parent = $this->tree->leafs[$pid];
		return end($parent->children) == $id;
	}

	
}