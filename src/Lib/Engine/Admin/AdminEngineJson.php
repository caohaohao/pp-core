<?php

namespace PP\Lib\Engine\Admin;

use Symfony\Component\DependencyInjection\ContainerAwareInterface;
use PXResponse;

/**
 * Class AdminEngineJson.
 *
 * @package PP\Lib\Engine\Admin
 */
class AdminEngineJson extends AbstractAdminEngine {

	protected $result;

	function initModules() {
		$this->area = $this->request->getArea();
		$this->modules = $this->getModule($this->app, $this->area);
	}

	function runModules() {
		// For correct user session expiration handling and admin auth module working
		if (!($this->hasAdminModules() || $this->area == $this->authArea)) {
			return;
		}

		$this->checkArea($this->area);

		$moduleDescription = $this->modules[$this->area];
		$instance = $moduleDescription->getModule();
		if ($instance instanceof ContainerAwareInterface) {
			$instance->setContainer($this->container);
		}

		$eventData = [
			'engine_type' => $this->engineType(),
			'engine_behavior' => $this->engineBehavior()
		];
		foreach ($this->app->triggers->system as $t) {
			$t->getTrigger()->onBeforeModuleRun($this, $moduleDescription, $eventData);
		}

		$this->result = $instance->adminJson();

		foreach ($this->app->triggers->system as $t) {
			$t->getTrigger()->onAfterModuleRun($this, $moduleDescription, $eventData);
		}
	}

	function sendJson() {
		$body = json_encode($this->result);

		$response = PXResponse::getInstance();
		$response->setContentType('text/javascript');
		$response->setLength(strlen($body));
		$response->send($body);
		exit;
	}

	/** {@inheritdoc} */
	public function engineBehavior() {
		return static::JSON_BEHAVIOR;
	}
}
