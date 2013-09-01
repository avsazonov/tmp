#ifndef GAMEMODE_H_INCLUDED
#define GAMEMODE_H_INCLUDED

class BattleField;
class GameControls;
class WorldCreator;
class WorldProcessor;
class FieldUnit;
class GameControl;

// ������ - ����� ����.
// ����� ����� ��������, processUserEvent ��� processStep ���������� ����� �����.
// ������ ���� ������� ������!
class GameMode {
protected:
	BattleField    * mBattleField;
	GameControls   * mGameControls;	
	WorldCreator   * mWorldCreator;

private:
	// ������ ������
	bool mDoExit;
	// ������ ������� ����
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

	// ����������, ����� ���� �� �����
	void makeExit() { mDoExit = true; }
	// ��������, ������� �� ����� ��� �� �����
	bool isAtExit() { return mDoExit; }

	// ����������, ����� ���� ��������� ����
	void makeRepeat() { mRepeat = true; }
	// ��������, ������� �� ����� ��� �������
	bool isAtRepeat() { return mRepeat; }

	// ������ ����������� ������ ������ ������
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

	// ����� ���������� ���� � ����
	class PreparingFieldMode : public GameMode {

		template<typename T>
		friend class tut::test_object;

		// ������� ���������� - ���� ������ �����
		TowerChoiceMenu * mTowerChoiceMenu;
		// ��������� ������� ������� ����������
		Tower           * mLastClickedTower;
		TowerSlot       * mLastClickedSlot;
		// ������ ������
		PlayButton      * mPlayButton;
	public:
		PreparingFieldMode(BattleField& battleField, 
			GameControls& gameControls, 
			WorldCreator& worldCreator) :
			GameMode(battleField, gameControls, worldCreator),
			mPlayButton(0), mLastClickedTower(0), mLastClickedSlot(0), mTowerChoiceMenu(0) {}
		
		// ���������� ������
		void initialize();

		// ��������� ���������������� �������
		GameMode * processUserEvent(float x, float y);
		
		// ������ �� ������
		GameMode * processStep(int timeDelta) { return this; }
	private:

		// ���������� � ���� �����
		void clearSlots();

		// �������� ����� �� ��������
		bool canStart();
	};

	// ����� ����� ����������
	class AttackingRunMode : public GameMode {
		template<typename T>
		friend class tut::test_object;

		// �� �� ��� � � ���������� ������ ������/�����
		GameControl * mPlayButton;
		// ������� ���������� - ����� ���������� ���������
		GameControl * mStatisticsBar;
		// ����� ����������� �������
		GameControl * mTimerBar;
		float         mTimeCounter;
		Randomizer	   mRandomizer;
	public:
		AttackingRunMode(BattleField& battleField, 
			GameControls& gameControls, 
			WorldCreator& worldCreator) :
			GameMode(battleField, gameControls, worldCreator), 
			mPlayButton(0), mStatisticsBar(0), mTimerBar(0), mRandomizer(worldCreator) {}

		// ����������-������������� (� �������� ��������� ����������)
		void initialize();

		// ��������� �������
		GameMode * processUserEvent(float x, float y);

		// ��� �������� ����
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

	// ����� ��������� ����������
	class GameStatisticsMode : public GameMode {
		// ������� ���������� ������ ��������� ����������
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

		// ���������� ������
		void initialize();

		// ��������� �����
		GameMode * processUserEvent(float x, float y);
		
		// ������ �� ����������
		GameMode * processStep(int timeDelta) { return this; }
	};
}

#endif
