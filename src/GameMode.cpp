#include "GameMode.h"
#include "FieldUnit.h"
#include "GameControls.h"
#include "WorldCreator.h"
#include "WorldProcessor.h"

using namespace TowerDefense;

void PreparingFieldMode::initialize() {
	// ������� ���� � ���� �� ������� �� �����
	mGameControls->addControl(mTowerChoiceMenu = new TowerChoiceMenu);
	mTowerChoiceMenu->disable();

	// ������ ������ ������ � ������ � �� �����
	mGameControls->addControl(mPlayButton = new PlayButton);

	// ����������������� ������ � ���������������� ���������,
	// ���� ����������, ����� ���� ������ ���� ��������� ��� ��������� ���� �����
	// mPlayButton->disable(); 
	mPlayButton->enable();
	
	mPlayButton->setSetting("pause", 1.f);
}

void PreparingFieldMode::clearSlots() {
	BattleField::BattleMap const * clickable_units = &mBattleField->getClickableUnits();

	// ������� ��� ����� � �����
	BattleField::TowerSlotsSet to_delete;
	// �������� �� ���� z ��� ������������� ��������
	for (BattleField::BattleMap::const_iterator layer_iterator = clickable_units->begin();
		layer_iterator != clickable_units->end();
		++layer_iterator)
		// �������� �� ���� �������� �� �����
		for (BattleField::UnitsOnLayer::const_iterator unit_iterator = layer_iterator->second.begin();
			unit_iterator != layer_iterator->second.end();
			++unit_iterator)
			// ���� ��������� ���� - ��������� � ������ �� ��������
			if ((*unit_iterator)->getObjectName().compare("TowerSlot") == 0)
				to_delete.insert(dynamic_cast<TowerSlot*>(*unit_iterator));

	// ���������� ��������
	for (BattleField::TowerSlotsSet::iterator unit_iterator = to_delete.begin();
		unit_iterator != to_delete.end();
		++unit_iterator)
		mBattleField->delTowerSlot(*unit_iterator);
}

bool PreparingFieldMode::canStart() {
	BattleField::BattleMap const * clickable_units = &mBattleField->getClickableUnits();
	
	// ��������, ����� �� ��������
	bool can_start = true;

	// ���� ��� �����
	for (BattleField::BattleMap::const_reverse_iterator layer_check = clickable_units->rbegin();
		layer_check != clickable_units->rend();
		++layer_check) {
		BattleField::UnitsOnLayer::const_iterator unit_check;
		for (unit_check = layer_check->second.begin();
			unit_check != layer_check->second.end();
			++unit_check)
			// ���� ����� ���� �� ���� - �������� �� �����
			if ((*unit_check)->getObjectName().compare("TowerSlot") == 0) {
				can_start = false;
				break;
			}
			if (!can_start)
				break;
	}

	return can_start;
}

// ���������� �������� ����������
// ����� ���� �� ������� � �� �����
// �� �� �������� �������� �� �����
GameMode * PreparingFieldMode::processUserEvent(float x, float y) {

		// �������� ������� ����. ��������� ����� ������ ���� ���� ������ ����� ���������
		if (mPlayButton->isEnabled() && !mTowerChoiceMenu->isEnabled()) {
			// �������� ������� �� ������
			if (mPlayButton->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y))) {
				// ������� �����
				mPlayButton->setSetting("pause", 0.f);
				// ������� ����� � �����
				clearSlots();
				// ��������� � ����� �����
				return new TowerDefense::AttackingRunMode(*mBattleField, *mGameControls, *mWorldCreator, *mWorldProcessor);
			}
		}

		BattleField::BattleMap const * clickable_units = &mBattleField->getClickableUnits();
		GameControls::ControlsList const * clickable_controls = &mGameControls->getClickableControls();

		// ������, ����� ������ �� ���� ��� �����
		if (!mTowerChoiceMenu->isEnabled()) {
			mLastClickedTower = 0;
			mLastClickedSlot  = 0;

			// ������� ��� ������������� ������� �� �����
			for (BattleField::BattleMap::const_reverse_iterator layer_iterator = clickable_units->rbegin();
				layer_iterator != clickable_units->rend();
				++layer_iterator) {
				for (BattleField::UnitsOnLayer::const_iterator unit_iterator = layer_iterator->second.begin();
					unit_iterator != layer_iterator->second.end();
					++unit_iterator) 

					// � ������, ������ �� �� ����
					if ((*unit_iterator)->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y))) {
						if ((*unit_iterator)->getObjectName().compare("TowerSlot") == 0)
							mLastClickedSlot = dynamic_cast<TowerSlot*>(*unit_iterator);
						else if ((*unit_iterator)->getObjectName().compare("Tower") == 0)
							mLastClickedTower = dynamic_cast<Tower*>(*unit_iterator);
						break;
					}

				// ���� �����, �������
				if (mLastClickedSlot || mLastClickedTower)
					break;
			}

			if ((0 == mLastClickedSlot) && (0 == mLastClickedTower)) 
				return this;

			// ���� ������ �� ����� ��� ���� - ���������� ����
			mTowerChoiceMenu->enable();

		} else {
			// ��� � ��� ������, ����� ���� �������, � �� ��� ����� ���������, ���� �� �� ������
			mTowerChoiceMenu->disable();

			// �������� ���� �� ����� ��� ���� �� ��������
			if ((0 == mLastClickedSlot) && (0 == mLastClickedTower)) 
				return this;

			// ��� ������ �� ���� ������ ���� �����. ���� ����-�� ��� - �� ������ ��� �� ��������
			if (mTowerChoiceMenu->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y))) {
				// ���������� ��� ����� ����������� ���� �� 3 ����� (�������, ������� � ������)
				int tower_type = 
					int(3 * (DrawHelper::getCellY(y) - mTowerChoiceMenu->getSetting("y").getValue()) / 
					        (mTowerChoiceMenu->getSetting("size_y").getValue()));

				// ���� ������� �����
				FieldUnit * mLastClickedUnit = mLastClickedSlot ? (FieldUnit*)mLastClickedSlot : (FieldUnit*)mLastClickedTower;
				float new_unit_x = mLastClickedUnit->getSetting("x").getValue(),
					  new_unit_y = mLastClickedUnit->getSetting("y").getValue();

				// ������ ������� �� ��� ��� ������ ����
				if (0 != mLastClickedSlot)
					mBattleField->delTowerSlot(mLastClickedSlot);
				else
					mBattleField->delTower(mLastClickedTower);
				
				TowerDefense::Tower *tower = 0;

				// ��������� ����� �� ����
				mBattleField->addTower(tower = mWorldCreator->createTower(WorldCreator::TOWERTYPE(tower_type)));
				if (tower) {
					tower->setSetting("x", new_unit_x);
					tower->setSetting("y", new_unit_y);
				}

				// ���������� ������ ������ (����� ��������)
				if (!mPlayButton->isEnabled()) 
					if (canStart())
						mPlayButton->enable();
			}
		}
	return this;
}

void AttackingRunMode::initialize() {
	// ���� ��� ������� ������ �����/����. ������� ���� � ���
	mPlayButton = mGameControls->getControlByName("PlayButton");
	if (!mPlayButton)
		mGameControls->addControl(mPlayButton = new PlayButton);
	mPlayButton->enable();

    // ���������� �� ����������� ���������
	mStatisticsBar = mGameControls->getControlByName("StatisticsBar");
	if (!mStatisticsBar)
		mGameControls->addControl(mStatisticsBar = new StatisticsBar);
	mStatisticsBar->enable();

	// � �������� �� ������
	mTimerBar = mGameControls->getControlByName("TimerBar");
	if (!mTimerBar)
		mGameControls->addControl(mTimerBar = new TimerBar);
	mTimerBar->enable();
}

GameMode * AttackingRunMode::processUserEvent(float x, float y) {
	// ������������ ������� �� ������ �����/����
	if (mPlayButton->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y))) 
		// ������ ������ � ��������� �� ���������������
		mPlayButton->setSetting("pause", 1.f - mPlayButton->getSetting("pause").getValue());
	return this;
}

const int gSecondsToAttack = 180;
const int gMaxRunOut = 4;

GameMode * AttackingRunMode::processStep(int timeDelta) {
	// ���� �� �� � �����, ��
	if (!int(mPlayButton->getSetting("pause").getValue())) {
		// ������� ����������, �������� �� ����� ���� ���������
		mStatisticsBar->setSetting("alive", float(mWorldProcessor->getAliveStatistics()[1]
		                                         +mWorldProcessor->getAliveStatistics()[2]
												 +mWorldProcessor->getAliveStatistics()[3]));

		// ��������� ����� ����
		if ((mWorldProcessor->getProcessTime() >= gSecondsToAttack) || (mStatisticsBar->getSetting("alive").getValue() > gMaxRunOut)) {
			mTimerBar->disable();
			mStatisticsBar->disable();
			// ������ ���� ����� - ������ ����� ����� �� ����� ����������
			return new GameStatisticsMode(*mBattleField, *mGameControls, *mWorldCreator, *mWorldProcessor);
		}

		// ���� �� ����� ���� - �� ������ ��� � ��������� ������
		mTimerBar->setSetting("minutes", float((gSecondsToAttack - int(mWorldProcessor->getProcessTime())) / 60));
		mTimerBar->setSetting("seconds", float((gSecondsToAttack - int(mWorldProcessor->getProcessTime())) % 60));

		// ���������� ��� ��� ����
		mWorldProcessor->generateStep(timeDelta);
	}

	return this;
}

void GameStatisticsMode::initialize() {
	GameControls::ControlsList * list = &mGameControls->getControls();
	if (list) {
		// ��� ������ ��������� ��� �������� ���������� ��� ���� � ���������� �������
		for (GameControls::ControlsList::iterator control_iterator = list->begin();
			control_iterator != list->end();
			++control_iterator)
			(*control_iterator)->disable();
	}

	// �������� ��� ������ ������� ���������� - ��������� ����������
	mFinalStatisticsBanner = mGameControls->getControlByName("GameFinalStatistics");
	if (!mFinalStatisticsBanner)
		mGameControls->addControl(mFinalStatisticsBanner = new TowerDefense::GameFinalStatistics);
	mFinalStatisticsBanner->enable();

	// ����� �� ���� ����� ������� ���� �� ���� ����� ������-������
	// � ���������� ��� ����� ���������
	mFinalStatisticsBanner->setSetting("killed_type_1", float(mWorldProcessor->getKilledStatistics()[1.f]));
	mFinalStatisticsBanner->setSetting("killed_type_2", float(mWorldProcessor->getKilledStatistics()[2.f]));
	mFinalStatisticsBanner->setSetting("killed_type_3", float(mWorldProcessor->getKilledStatistics()[3.f]));
	mFinalStatisticsBanner->setSetting("alive", float(mWorldProcessor->getAliveStatistics()[1.f] + 
													  mWorldProcessor->getAliveStatistics()[2.f] + 
													  mWorldProcessor->getAliveStatistics()[3.f]
	));

	// ������, � ������ ���� ������� �� ����� ��� ��������
	mFinalStatisticsBanner->setSetting("win", 
		float(mFinalStatisticsBanner->getSetting("alive").getValue() < (gMaxRunOut + 1))
	);
}

GameMode * GameStatisticsMode::processUserEvent(float x, float y) {
	// ��������� �������� ������ �������� ����������
	// ���� �������� �� ������� - ��������� ����
	// ����� ������ �������
	if (!mFinalStatisticsBanner->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y)))
		makeExit();
	else {
		makeExit();
		makeRepeat();
	}
	return this;
}
