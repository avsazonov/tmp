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

private:
	// маркер выхода
	bool mDoExit;
	// маркер повтора игры
	bool mRepeat;

public:
	GameMode(BattleField& battleField, 
		GameControls& gameControls, 
		WorldCreator& worldCreator) :
		mBattleField(&battleField),
		mGameControls(&gameControls),
		mWorldCreator(&worldCreator),
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
#include "WorldProcessor.h"

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
			WorldCreator& worldCreator) :
			GameMode(battleField, gameControls, worldCreator),
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
		template<typename T>
		friend class tut::test_object;

		// Та же что и в подготовке кнопка старта/паузы
		GameControl * mPlayButton;
		// Элемент управления - вывод статистики убежавших
		GameControl * mStatisticsBar;
		// Вывод оставшегося времени
		GameControl * mTimerBar;
		float         mTimeCounter;
		Randomizer	   mRandomizer;
	public:
		AttackingRunMode(BattleField& battleField, 
			GameControls& gameControls, 
			WorldCreator& worldCreator) :
			GameMode(battleField, gameControls, worldCreator), 
			mPlayButton(0), mStatisticsBar(0), mTimerBar(0), mRandomizer(worldCreator) {}

		// подготовка-инициализация (в основном элементов управления)
		void initialize();

		// обработка нажатий
		GameMode * processUserEvent(float x, float y);

		// шаг игрового мира
		GameMode * processStep(int timeDelta); 

		typedef std::map<float, int> StatisticsMap;

	private:
		StatisticsMap mKilledStatistics, mAliveStatistics;
		void generateStep(int timeDelta);

		void processEnemies(int timeDelta);
		void processTowers(int timeDelta);
		void processShots(int timeDelta);

	public:
		virtual StatisticsMap& getKilledStatistics() { return mKilledStatistics; }
		virtual StatisticsMap& getAliveStatistics()  { return mAliveStatistics;  }
		virtual void updateKilled(float type) { ++mKilledStatistics[type];  }
		virtual void updateAlive(float type)  { ++mAliveStatistics[type];   }
	};

	// Вывод финальной статистики
	class GameStatisticsMode : public GameMode {
		// элемент управления вывода финальной статистики
		GameControl * mFinalStatisticsBanner;
		AttackingRunMode::StatisticsMap mAliveStatistics, mKilledStatistics;
	public:

		GameStatisticsMode(BattleField& battleField, 
			GameControls& gameControls, 
			WorldCreator& worldCreator,
			AttackingRunMode::StatisticsMap aliveStatistics,
			AttackingRunMode::StatisticsMap killedStatistics) :
			GameMode(battleField, gameControls, worldCreator), 
			mFinalStatisticsBanner(0), mAliveStatistics(aliveStatistics), mKilledStatistics(killedStatistics) {}

		// подготовка режима
		void initialize();

		// обработка ввода
		GameMode * processUserEvent(float x, float y);
		
		// ничего не происходит
		GameMode * processStep(int timeDelta) { return this; }
	};
}

#endif
