<?php
class PXTabbedMenuRegionChanger extends PXAdminWidget {
	function html() {
			$html = '';
			if(
				isset(PXRegistry::getApp()->types[PXPluginMultiRegions::REGION_MARK]) && //FIXME: in the future this behaviour will be managed via plugins abstraction
				PXRegistry::getUser()->can('manage_region') &&
				PXRegistry::getLayout()->outerLayout === 'index'
			){
				$regions = PXRegistry::getApp()->plugins['multipleregions'];
				if($regionsData = $regions->getRegionsAsDirectory() ){
					$html             .= sprintf('<span style="margin: 0 0 0 5px; color: #fff;">�������� ������:</span><br/><div id="region_selector" style="display: inline-block;"><form method="POST" action="%s">&nbsp;', PXRegistry::getRequest()->GetRequestUri());
					$varName           = PXPluginMultiRegions::REGION_MARK . '[rid]';
					$regionsData[null] = '- �������� ��� ������� -';
					$regionSelector    = new PXInputDropDown($varName, $regions->getRid(), $regionsData);
					$html             .= $regionSelector->html();
					$html             .= sprintf('</div></form><script>document.getElementById("region_selector").firstChild.elements["%s"].onchange = function(){ this.form.submit(); return true;} </script>', $varName);
				}
			}
			return $html;
		}
	}
?>