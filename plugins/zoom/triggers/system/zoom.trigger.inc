<?php

class PXTriggerSystemZoom extends PXAbstractSystemTrigger {
	public $app;
	public $layout;

	function onAfterEngineStart($engine) {
		PXRegistry::assignToObject($this);

		if (!$this->app->isAdminEngine() && $this->layout instanceof PXUserHTMLLayout) {
			$this->layout->addFilter(array('PXZoomFilter', 'run'));
		}
	}
}

?>
