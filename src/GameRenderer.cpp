#include "GameRenderer.h"
#include "FieldUnit.h"
#include "GameControls.h"

void GameRenderer::renderBattleAndControls() const {

	BattleField::BattleMap const * pBattleMap = &mBattleField->getBattleMap();
	
	BattleField::BattleMap::const_iterator    current_layer_entry;
	BattleField::UnitsOnLayer::const_iterator current_unit_iterator;

	// ���� �� ���� ������� (std::map ����������� �� ������ �� ���������)
	// ����� - ��� ����
	for (current_layer_entry = pBattleMap->begin(); 
		current_layer_entry != pBattleMap->end(); 
		++current_layer_entry)
		// �������� �� ���� �������� �� ������
		for (current_unit_iterator = current_layer_entry->second.begin(); 
			current_unit_iterator != current_layer_entry->second.end();
			++current_unit_iterator)
			// ������������ ������. 
			// layer = 0 ������������� ������������
			(*current_unit_iterator)->render();

	// ������ ������ ������ �������� �������� ����������
	GameControls::ControlsList::const_iterator current_control_iterator;
	GameControls::ControlsList const * controls = &mControls->getControls();
	for (current_control_iterator = controls->begin(); 
		current_control_iterator != controls->end(); 
		++current_control_iterator)
		// ���������� ������� ���������� ������ ���� �� �������
		if ((*current_control_iterator)->isEnabled())
			(*current_control_iterator)->render();
}
