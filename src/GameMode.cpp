#include "GameMode.h"
#include "FieldUnit.h"
#include "GameControls.h"
#include "WorldCreator.h"
#include "WorldProcessor.h"

using namespace TowerDefense;

void PreparingFieldMode::initialize() {
	// создаем меню и пока не выводим на экран
	mGameControls->addControl(mTowerChoiceMenu = new TowerChoiceMenu);
	mTowerChoiceMenu->disable();

	// создаём кнопку старта и ставим её на паузу
	mGameControls->addControl(mPlayButton = new PlayButton);

	// Расскоментировать строку и закомментировать следующую,
	// если необходимо, чтобы игру нельзя было запустить без установки всех башен
	// mPlayButton->disable(); 
	mPlayButton->enable();
	
	mPlayButton->setSetting("pause", 1.f);
}

void PreparingFieldMode::clearSlots() {
	BattleField::BattleMap const * clickable_units = &mBattleField->getClickableUnits();

	// Убираем все слоты с карты
	BattleField::TowerSlotsSet to_delete;
	// проходим по всем z для нажимабельных объектов
	for (BattleField::BattleMap::const_iterator layer_iterator = clickable_units->begin();
		layer_iterator != clickable_units->end();
		++layer_iterator)
		// проходим по всем объектам на карте
		for (BattleField::UnitsOnLayer::const_iterator unit_iterator = layer_iterator->second.begin();
			unit_iterator != layer_iterator->second.end();
			++unit_iterator)
			// если встретили слот - добавляем в список на удаление
			if ((*unit_iterator)->getObjectName().compare("TowerSlot") == 0)
				to_delete.insert(dynamic_cast<TowerSlot*>(*unit_iterator));

	// собственно удаление
	for (BattleField::TowerSlotsSet::iterator unit_iterator = to_delete.begin();
		unit_iterator != to_delete.end();
		++unit_iterator)
		mBattleField->delTowerSlot(*unit_iterator);
}

bool PreparingFieldMode::canStart() {
	BattleField::BattleMap const * clickable_units = &mBattleField->getClickableUnits();
	
	// проверка, можно ли начинать
	bool can_start = true;

	// ищем все слоты
	for (BattleField::BattleMap::const_reverse_iterator layer_check = clickable_units->rbegin();
		layer_check != clickable_units->rend();
		++layer_check) {
		BattleField::UnitsOnLayer::const_iterator unit_check;
		for (unit_check = layer_check->second.begin();
			unit_check != layer_check->second.end();
			++unit_check)
			// если нашли хотя бы один - начинать не можем
			if ((*unit_check)->getObjectName().compare("TowerSlot") == 0) {
				can_start = false;
				break;
			}
			if (!can_start)
				break;
	}

	return can_start;
}

// достаточно объемный обработчик
// можно было бы разбить и на части
// но он целостно отвечает за режим
GameMode * PreparingFieldMode::processUserEvent(float x, float y) {

		// Проверка запуска игры. Запустить можно только если меню выбора башни неактивно
		if (mPlayButton->isEnabled() && !mTowerChoiceMenu->isEnabled()) {
			// Проверка нажатия на кнопку
			if (mPlayButton->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y))) {
				// Убираем паузу
				mPlayButton->setSetting("pause", 0.f);
				// убираем слоты с карты
				clearSlots();
				// переходим в режим атаки
				return new TowerDefense::AttackingRunMode(*mBattleField, *mGameControls, *mWorldCreator, *mWorldProcessor);
			}
		}

		BattleField::BattleMap const * clickable_units = &mBattleField->getClickableUnits();
		GameControls::ControlsList const * clickable_controls = &mGameControls->getClickableControls();

		// случай, когда нажали на слот или башню
		if (!mTowerChoiceMenu->isEnabled()) {
			mLastClickedTower = 0;
			mLastClickedSlot  = 0;

			// обходим все нажимабельные объекты на карте
			for (BattleField::BattleMap::const_reverse_iterator layer_iterator = clickable_units->rbegin();
				layer_iterator != clickable_units->rend();
				++layer_iterator) {
				for (BattleField::UnitsOnLayer::const_iterator unit_iterator = layer_iterator->second.begin();
					unit_iterator != layer_iterator->second.end();
					++unit_iterator) 

					// и узнаем, нажали ли на него
					if ((*unit_iterator)->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y))) {
						if ((*unit_iterator)->getObjectName().compare("TowerSlot") == 0)
							mLastClickedSlot = dynamic_cast<TowerSlot*>(*unit_iterator);
						else if ((*unit_iterator)->getObjectName().compare("Tower") == 0)
							mLastClickedTower = dynamic_cast<Tower*>(*unit_iterator);
						break;
					}

				// если нашли, выходим
				if (mLastClickedSlot || mLastClickedTower)
					break;
			}

			if ((0 == mLastClickedSlot) && (0 == mLastClickedTower)) 
				return this;

			// если нажали на башню или слот - активируем меню
			mTowerChoiceMenu->enable();

		} else {
			// тут у нас случай, когда меню активно, и мы его тогда выключаем, куда бы не нажали
			mTowerChoiceMenu->disable();

			// особенно если на башню или слот не нажимали
			if ((0 == mLastClickedSlot) && (0 == mLastClickedTower)) 
				return this;

			// нам нажали на меню выбора типа башни. если куда-то еще - то ничего уже не делается
			if (mTowerChoiceMenu->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y))) {
				// определяем тип башни разделением меню на 3 части (верхнюю, среднюю и нижнюю)
				int tower_type = 
					int(3 * (DrawHelper::getCellY(y) - mTowerChoiceMenu->getSetting("y").getValue()) / 
					        (mTowerChoiceMenu->getSetting("size_y").getValue()));

				// куда ставить башню
				FieldUnit * mLastClickedUnit = mLastClickedSlot ? (FieldUnit*)mLastClickedSlot : (FieldUnit*)mLastClickedTower;
				float new_unit_x = mLastClickedUnit->getSetting("x").getValue(),
					  new_unit_y = mLastClickedUnit->getSetting("y").getValue();

				// всегда удаляем то что там раньше было
				if (0 != mLastClickedSlot)
					mBattleField->delTowerSlot(mLastClickedSlot);
				else
					mBattleField->delTower(mLastClickedTower);
				
				TowerDefense::Tower *tower = 0;

				// добавляем башню на поле
				mBattleField->addTower(tower = mWorldCreator->createTower(WorldCreator::TOWERTYPE(tower_type)));
				if (tower) {
					tower->setSetting("x", new_unit_x);
					tower->setSetting("y", new_unit_y);
				}

				// активируем кнопку старта (после проверки)
				if (!mPlayButton->isEnabled()) 
					if (canStart())
						mPlayButton->enable();
			}
		}
	return this;
}

void AttackingRunMode::initialize() {
	// ищем уже готовую кнопку старт/стоп. создаем если её нет
	mPlayButton = mGameControls->getControlByName("PlayButton");
	if (!mPlayButton)
		mGameControls->addControl(mPlayButton = new PlayButton);
	mPlayButton->enable();

    // аналогично со статистикой убежавших
	mStatisticsBar = mGameControls->getControlByName("StatisticsBar");
	if (!mStatisticsBar)
		mGameControls->addControl(mStatisticsBar = new StatisticsBar);
	mStatisticsBar->enable();

	// и таймером на экране
	mTimerBar = mGameControls->getControlByName("TimerBar");
	if (!mTimerBar)
		mGameControls->addControl(mTimerBar = new TimerBar);
	mTimerBar->enable();
}

GameMode * AttackingRunMode::processUserEvent(float x, float y) {
	// обрабатываем нажатия на кнопку старт/стоп
	if (mPlayButton->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y))) 
		// просто меняем её состояние на противоположное
		mPlayButton->setSetting("pause", 1.f - mPlayButton->getSetting("pause").getValue());
	return this;
}

const int gSecondsToAttack = 180;
const int gMaxRunOut = 4;

GameMode * AttackingRunMode::processStep(int timeDelta) {
	// если мы не в паузе, то
	if (!int(mPlayButton->getSetting("pause").getValue())) {
		// выводим статистику, суммируя по типам всех сбежавших
		mStatisticsBar->setSetting("alive", float(mWorldProcessor->getAliveStatistics()[1]
		                                         +mWorldProcessor->getAliveStatistics()[2]
												 +mWorldProcessor->getAliveStatistics()[3]));

		// проверяем конец игры
		if ((mWorldProcessor->getProcessTime() >= gSecondsToAttack) || (mStatisticsBar->getSetting("alive").getValue() > gMaxRunOut)) {
			mTimerBar->disable();
			mStatisticsBar->disable();
			// вобщем если конец - меняем режим атаки на режим завершения
			return new GameStatisticsMode(*mBattleField, *mGameControls, *mWorldCreator, *mWorldProcessor);
		}

		// если не конец игры - то делаем шаг и обновляем таймер
		mTimerBar->setSetting("minutes", float((gSecondsToAttack - int(mWorldProcessor->getProcessTime())) / 60));
		mTimerBar->setSetting("seconds", float((gSecondsToAttack - int(mWorldProcessor->getProcessTime())) % 60));

		// собственно сам шаг игры
		mWorldProcessor->generateStep(timeDelta);
	}

	return this;
}

void GameStatisticsMode::initialize() {
	GameControls::ControlsList * list = &mGameControls->getControls();
	if (list) {
		// для начала отключаем все элементы управления что были в предыдущих режимах
		for (GameControls::ControlsList::iterator control_iterator = list->begin();
			control_iterator != list->end();
			++control_iterator)
			(*control_iterator)->disable();
	}

	// получаем или создаём элемент управления - финальную статистику
	mFinalStatisticsBanner = mGameControls->getControlByName("GameFinalStatistics");
	if (!mFinalStatisticsBanner)
		mGameControls->addControl(mFinalStatisticsBanner = new TowerDefense::GameFinalStatistics);
	mFinalStatisticsBanner->enable();

	// здесь по идее можно сделать цикл по всем типам юнитов-врагов
	// и передавать его через настройки
	mFinalStatisticsBanner->setSetting("killed_type_1", float(mWorldProcessor->getKilledStatistics()[1.f]));
	mFinalStatisticsBanner->setSetting("killed_type_2", float(mWorldProcessor->getKilledStatistics()[2.f]));
	mFinalStatisticsBanner->setSetting("killed_type_3", float(mWorldProcessor->getKilledStatistics()[3.f]));
	mFinalStatisticsBanner->setSetting("alive", float(mWorldProcessor->getAliveStatistics()[1.f] + 
													  mWorldProcessor->getAliveStatistics()[2.f] + 
													  mWorldProcessor->getAliveStatistics()[3.f]
	));

	// победа, в случае если убежало не более чем положено
	mFinalStatisticsBanner->setSetting("win", 
		float(mFinalStatisticsBanner->getSetting("alive").getValue() < (gMaxRunOut + 1))
	);
}

GameMode * GameStatisticsMode::processUserEvent(float x, float y) {
	// действуем согласно логике элемента управления
	// если кликнули по баннеру - повторяем игру
	// иначе просто выходим
	if (!mFinalStatisticsBanner->isMyAreaClicked(DrawHelper::getCellX(x), DrawHelper::getCellY(y)))
		makeExit();
	else {
		makeExit();
		makeRepeat();
	}
	return this;
}
