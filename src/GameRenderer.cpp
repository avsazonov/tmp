#include "GameRenderer.h"
#include "FieldUnit.h"
#include "GameControls.h"

void GameRenderer::renderBattleAndControls() const {

	BattleField::BattleMap const * pBattleMap = &mBattleField->getBattleMap();
	
	BattleField::BattleMap::const_iterator    current_layer_entry;
	BattleField::UnitsOnLayer::const_iterator current_unit_iterator;

	// Öèêë ïî âñåì óğîâíÿì (std::map ñîğòèğîâàíà ïî êëş÷àì ïî óìîë÷àíèş)
	// Êëş÷è - ıòî ñëîè
	for (current_layer_entry = pBattleMap->begin(); 
		current_layer_entry != pBattleMap->end(); 
		++current_layer_entry)
		// Ïğîõîäèì ïî âñåì îáúåêòàì íà óğîâíå
		for (current_unit_iterator = current_layer_entry->second.begin(); 
			current_unit_iterator != current_layer_entry->second.end();
			++current_unit_iterator)
			// Îòğèñîâûâàåì îáúåêò. 
			// layer = 0 àâòîìàòè÷åñêè ïğîïóñêàşòñÿ
			(*current_unit_iterator)->render();

	// Âñåãäà ïîâåğõ ğèñóåì àêòèâíûå ıëåìåíòû óïğàâëåíèÿ
	GameControls::ControlsList::const_iterator current_control_iterator;
	GameControls::ControlsList const * controls = &mControls->getControls();
	for (current_control_iterator = controls->begin(); 
		current_control_iterator != controls->end(); 
		++current_control_iterator)
		// Îòîáğàæàåì ıëåìåíò óïğàâëåíèÿ òîëüêî åñëè îí àêòèâåí
		if ((*current_control_iterator)->isEnabled())
			(*current_control_iterator)->render();
}
