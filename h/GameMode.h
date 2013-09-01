#ifndef GAMEMODE_H_INCLUDED
#define GAMEMODE_H_INCLUDED

class BattleField;
class GameControls;
class WorldCreator;
class WorldProcessor;
class FieldUnit;
class GameControl;

// Объект - режим игры.
// Когда режим меняется, processUserEvent или processStep возвращает новый режим.
// Режимы НАДО удалять самому!
class GameMode {
protected:
	BattleField    * mBattleField;
	GameControls   * mGameControls;	
	WorldCreator   * mWorldCreator;
	WorldProcessor * mWorldProcessor;

private:
	// маркер выхода
	bool mDoExit;
	// маркер повтора игры
	bool mRepeat;

public:
	GameMode(BattleField& battleField, 
		GameControls& gameControls, 
		WorldCreator& worldCreator,
		WorldProcessor& worldProcessor) :
		mBattleField(&battleField),
		mGameControls(&gameControls),
		mWorldCreator(&worldCreator),
		mWorldProcessor(&worldProcessor),
		mDoExit(false), mRepeat(false)
	{}

	// вызывается, когда надо на выход
	void makeExit() { mDoExit = true; }
	// проверка, помечен ли режим как на выход
	bool isAtExit() { return mDoExit; }

	// вызывается, когда надо повторять игру
	void makeRepeat() { mRepeat = true; }
	// проверка, помечен ли режим для повтора
	bool isAtRepeat() { return mRepeat; }

	// Чистые виртуальные методы работы режима
	virtual void initialize() = 0;
	virtual GameMode * processUserEvent(float x, float y) = 0;
	virtual GameMode * processStep(int timeDelta) = 0;

};

#include "FieldUnit.h"

namespace tut
{
    template<typename T>
    class test_object;
}

namespace TowerDefense {

	class TowerChoiceMenu;
	class PlayButton;

	// Режим подготовки поля к игре
	class PreparingFieldMode : public GameMode {

		template<typename T>
		friend class tut::test_object;

		// элемент управления - меню выбора башни
		TowerChoiceMenu * mTowerChoiceMenu;
		// последний нажатый элемент управления
		Tower           * mLastClickedTower;
		TowerSlot       * mLastClickedSlot;
		// кнопка старта
		PlayButton      * mPlayButton;
	public:
		PreparingFieldMode(BattleField& battleField, 
			GameControls& gameControls, 
			WorldCreator& worldCreator,
			WorldProcessor& worldProcessor) :
			GameMode(battleField, gameControls, worldCreator, worldProcessor),
			mPlayButton(0), mLastClickedTower(0), mLastClickedSlot(0), mTowerChoiceMenu(0) {}
		
		// подготовка режима
		void initialize();

		// обработка пользовательских событий
		GameMode * processUserEvent(float x, float y);
		
		// ничего не делаем
		GameMode * processStep(int timeDelta) { return this; }
	private:

		// подчистить с поля слоты
		void clearSlots();

		// проверка можно ли начинать
		bool canStart();
	};

	// Режим атаки противника
	class AttackingRunMode : public GameMode {
		// Та же что и в подготовке кнопка старта/паузы
		GameControl * mPlayButton;
		// Элемент управления - вывод статистики убежавших
		GameControl * mStatisticsBar;
		// Вывод оставшегося времени
		GameControl * mTimerBar;
	public:
		AttackingRunMode(BattleField& battleField, 
			GameControls& gameControls, 
			WorldCreator& worldCreator,
			WorldProcessor& worldProcessor) :
			GameMode(battleField, gameControls, worldCreator, worldProcessor), 
			mPlayButton(0), mStatisticsBar(0), mTimerBar(0) {}

		// подготовка-инициализация (в основном элементов управления)
		void initialize();

		// обработка нажатий
		GameMode * processUserEvent(float x, float y);

		// шаг игрового мира
		GameMode * processStep(int timeDelta); 
	};

	// Вывод финальной статистики
	class GameStatisticsMode : public GameMode {
		// элемент управления вывода финальной статистики
		GameControl * mFinalStatisticsBanner;
	public:
		GameStatisticsMode(BattleField& battleField, 
			GameControls& gameControls, 
			WorldCreator& worldCreator,
			WorldProcessor& worldProcessor) :
			GameMode(battleField, gameControls, worldCreator, worldProcessor), 
			mFinalStatisticsBanner(0) {}

		// подготовка режима
		void initialize();

		// обработка ввода
		GameMode * processUserEvent(float x, float y);
		
		// ничего не происходит
		GameMode * processStep(int timeDelta) { return this; }
	};
}

#endif
