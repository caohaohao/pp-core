<?php
	class PXAdminTableCommonControls extends PXAdminWidget {
		function html(){
			return <<<HTML
				<div class="multiple-bottom-controls">
					<a href="#doCommonMultipleDelete"       confirm="�� ������� ��� ������ ������� ���������� �������?">�������</a>
					<a href="#doCommonMultipleStatusChange" helper="true">�������� ������</a>
					<div class="helper">
						<div class="status-changer">
							<button class="on"  assign='{"status": "true"}'>������������</button>
							<button class="off" assign='{"status": "false"}'>������</button>
						</div>
					</div>
				</div>
HTML;
		}
	}
?>