<?php

namespace PP\Module;

require_once PPLIBPATH . '/Security/blockingnumbers.class.inc';

/**
 * Class AuthModule.
 *
 * @package PP\Module
 */
class AuthModule extends AbstractModule {

	function adminIndex() {

		$captcha = new \NLBlockingNumbers();

		$captchaKey = $captcha->CreateNew()
			? $captcha->getKey()
			: 0;

		$captchaNote = $this->app->isDevelopmentMode()
			? 'капча не требуется'
			: '';

		$this->layout->setLoginForm(
			'action.phtml',
			'POST',
			[
				'login' => 'login',
				'passwd' => 'passwd',
				'referer' => 'referer',
				'area' => 'area',
				'captchaKey' => 'captchaKey',
				'captchaVal' => 'captchaVal',
			],
			[
				'login' => $this->user->login,
				'passwd' => NULL,
				'referer' => $this->request->getReferer(),
				'area' => 'auth',
				'captchaKey' => $captchaKey,
				'captchaNote' => $captchaNote,
			]
		);

		if ($this->user->IsAuthed()) {
			$this->user->unAuth();
			$this->layout->assign(
				'LOGIN.ERROR',
				'<strong class="login-error">Нет доступа</strong>'
			);


		} else if ($this->request->GetAction() == 'error') {
			$this->layout->assign(
				'LOGIN.ERROR',
				'<strong class="login-error">Ошибка авторизации</strong>'
			);
		}
	}

	function adminAction() {

		// TODO: refactor, accept referrer for current domain only
		// TODO: remove this shitty magic with nextLocation guessing

		$captcha = new \NLBlockingNumbers();
		$captchaKey = $this->request->getPostVar('captchaKey');
		$captchaVal = $this->request->getPostVar('captchaVal');

		$captchaSolved = $this->app->isDevelopmentMode();
		if ($captcha->CheckValueByKey($captchaKey, $captchaVal, true)) {
			$captchaSolved = true;
		}

		if (($captchaSolved === true) && ($this->_auth() == 0)) {
			$nextLocation = $this->request->getReferer();

		} else {
			$nextLocation = $this->request->getReferer();

			if (strpos($nextLocation, '?') !== FALSE) {
				if (strpos($nextLocation, '?action=') !== FALSE || strpos($nextLocation, '&action=') !== FALSE) {
					$nextLocation = preg_replace("/(?<=\&|\?)action=[^&]*/", "action=error", $nextLocation);
				} else {
					$nextLocation .= '&action=error';
				}

			} else {
				$nextLocation .= '?action=error';
			}
		}

		$this->response->redirect($nextLocation);
	}

	function userAction() {
		if ($this->_auth() == 0) {
			$nextLocation = ($this->request->GetVar('onsuccess'))
				? $this->request->GetVar('onsuccess')
				: $this->request->GetReferer();

			$nextLocation = removeParamFromUrl($nextLocation, 'login');

		} else {
			$nextLocation = ($this->request->GetVar('onerror'))
				? $this->request->GetVar('onerror')
				: $this->request->GetReferer();

			$nextLocation = appendParamToUrl($nextLocation, 'login', 'bad');
		}
		return $nextLocation;
	}


	function _auth() {
		$audit = \PXAuditLogger::getLogger();

		switch ($this->request->getVar('action')) {
			case 'exit':
				$audit->info('Выход из системы', 'suser' . '/' . $this->user->id);
				$this->user->UnAuth();
				return 0;
				break;

			default:
				if ($this->user->IsAuthed()) {
					$this->user->auth();
					$audit->info('Вход в систему', 'suser' . '/' . $this->user->id);
					return 0;

				} else {
					$audit->error('Неудачный вход в систему', 'suser' . '/', 'ERROR');
					return 1;
				}
				break;
		}
	}
}
