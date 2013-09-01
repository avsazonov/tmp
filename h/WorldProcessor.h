#ifndef WORLDPROCESSOR_H_INCLUDED
#define WORLDPROCESSOR_H_INCLUDED

#include <map>
#include <list>

class WorldCreator;

#include "FieldUnit.h"

namespace tut
{
    template<typename T>
    class test_object;
}

// ������� �������� �� ��������� 
// ����� ������
class Randomizer {
	float mLastCreation, mTimeCounter, mRandomTime;
	WorldCreator * mWorldCreator;
public:
	Randomizer(WorldCreator & worldCreator) :
	  mWorldCreator(&worldCreator), 
		  mLastCreation(0), mTimeCounter(0), mRandomTime(0) {}

	typedef std::list<TowerDefense::Enemy*> UnitsList;

	// ���������� ������ ������ ��� ���������� �� �����
	UnitsList createRandomUnits(int timeDelta);
};

// �������� �� ������� �����/������
class WorldProcessor {

	template<typename T>
    friend class tut::test_object;

	BattleField  * mBattleField;
	WorldCreator * mWorldCreator;
	float          mTimeCounter;
	bool           mStatisticsInited;
	Randomizer	   mRandomizer;
public:
	WorldProcessor(WorldCreator& worldCreator, BattleField& battleField):
	  mBattleField(&battleField), 
	  mWorldCreator(&worldCreator), 
	  mTimeCounter(0), mRandomizer(worldCreator), mStatisticsInited(false) {}

	// ������� ��� �������� ����
	virtual void   generateStep(int timeDelta);
	
	// ��������� �����, � ��������
	virtual float  getProcessTime() const;
	
	typedef std::map<float, int> StatisticsMap;

private:
	StatisticsMap mKilledStatistics, mAliveStatistics;

public:

	// ����� ����������, ��� <--> ���������� (������, ���������)
	virtual StatisticsMap& getKilledStatistics() { return mKilledStatistics; }
	virtual StatisticsMap& getAliveStatistics()  { return mAliveStatistics;  }

protected:
	// �����������. ������� ������ ������� � �����
	virtual void processEnemies(int timeDelta);
	virtual void processShots(int timeDelta);
	virtual void processTowers(int timeDelta);

	// refactoring, extracting processTowers
	virtual void doTowerShot(TowerDefense::Tower * tower, const BattleField::EnemiesSet &enemies, int timeDelta);
	
	// refactoring, extracting processShots. 
	virtual bool doOneShot(TowerDefense::Shot * shot, int timeDelta); // returns true if needed to delete shot
	virtual void moveShot(TowerDefense::Shot * shot, int timeDelta);

	// refactoring, moving enemies
	virtual void moveEnemy(TowerDefense::Enemy * enemy, int timeDelta);

	// ����������� ���������� �� �������
	virtual void updateKilled(float type);
	virtual void updateAlive(float type);

	// ���������� std::map ������
	virtual void initStatistics();

	typedef std::pair<float, float> StepPoint;

};

#endif
