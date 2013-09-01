#include "GameRenderer.h"
#include "FieldUnit.h"
#include "GameControls.h"

void GameRenderer::renderBattleAndControls() const {

	for (BattleField::BackGroundCellsSet::iterator bgcell_iterator = mBattleField->getBackGroundCells().begin();
		bgcell_iterator != mBattleField->getBackGroundCells().end();
		++bgcell_iterator)
		(*bgcell_iterator)->render();

	for (BattleField::PathCellsSet::iterator pathcell_iterator = mBattleField->getPathCells().begin();
		pathcell_iterator != mBattleField->getPathCells().end();
		++pathcell_iterator)
		(*pathcell_iterator)->render();

	for (BattleField::TowerSlotsSet::iterator towerslot_iterator = mBattleField->getTowerSlots().begin();
		towerslot_iterator != mBattleField->getTowerSlots().end();
		++towerslot_iterator)
		(*towerslot_iterator)->render();

	for (BattleField::TowersSet::iterator tower_iterator = mBattleField->getTowers().begin();
		tower_iterator != mBattleField->getTowers().end();
		++tower_iterator)
		(*tower_iterator)->render();

	for (BattleField::EnemiesSet::iterator enemy_iterator = mBattleField->getEnemies().begin();
		enemy_iterator != mBattleField->getEnemies().end();
		++enemy_iterator)
		(*enemy_iterator)->render();

	for (BattleField::ShotsSet::iterator shot_iterator = mBattleField->getShots().begin();
		shot_iterator != mBattleField->getShots().end();
		++shot_iterator)
		(*shot_iterator)->render();

	// Всегда поверх рисуем активные элементы управления
	GameControls::ControlsList::const_iterator current_control_iterator;
	GameControls::ControlsList const * controls = &mControls->getControls();
	for (current_control_iterator = controls->begin(); 
		current_control_iterator != controls->end(); 
		++current_control_iterator)
		// Отображаем элемент управления только если он активен
		if ((*current_control_iterator)->isEnabled())
			(*current_control_iterator)->render();
}
