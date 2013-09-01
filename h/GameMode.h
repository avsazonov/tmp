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
	WorldProcessor * mWorldProcessor;

private:
	// ������ ������
	bool mDoExit;
	// ������ ������� ����
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
			WorldCreator& worldCreator,
			WorldProcessor& worldProcessor) :
			GameMode(battleField, gameControls, worldCreator, worldProcessor),
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
		// �� �� ��� � � ���������� ������ ������/�����
		GameControl * mPlayButton;
		// ������� ���������� - ����� ���������� ���������
		GameControl * mStatisticsBar;
		// ����� ����������� �������
		GameControl * mTimerBar;
	public:
		AttackingRunMode(BattleField& battleField, 
			GameControls& gameControls, 
			WorldCreator& worldCreator,
			WorldProcessor& worldProcessor) :
			GameMode(battleField, gameControls, worldCreator, worldProcessor), 
			mPlayButton(0), mStatisticsBar(0), mTimerBar(0) {}

		// ����������-������������� (� �������� ��������� ����������)
		void initialize();

		// ��������� �������
		GameMode * processUserEvent(float x, float y);

		// ��� �������� ����
		GameMode * processStep(int timeDelta); 
	};

	// ����� ��������� ����������
	class GameStatisticsMode : public GameMode {
		// ������� ���������� ������ ��������� ����������
		GameControl * mFinalStatisticsBanner;
	public:
		GameStatisticsMode(BattleField& battleField, 
			GameControls& gameControls, 
			WorldCreator& worldCreator,
			WorldProcessor& worldProcessor) :
			GameMode(battleField, gameControls, worldCreator, worldProcessor), 
			mFinalStatisticsBanner(0) {}

		// ���������� ������
		void initialize();

		// ��������� �����
		GameMode * processUserEvent(float x, float y);
		
		// ������ �� ����������
		GameMode * processStep(int timeDelta) { return this; }
	};
}

#endif
