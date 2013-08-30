#ifndef WORLDPROCESSOR_H_INCLUDED
#define WORLDPROCESSOR_H_INCLUDED

#include <map>
#include <list>

class WorldCreator;
class FieldUnit;
class BattleField;

// ������� �������� �� ��������� 
// ����� ������
class Randomizer {
	float mLastCreation, mTimeCounter, mRandomTime;
	WorldCreator * mWorldCreator;
public:
	Randomizer(WorldCreator & worldCreator) :
	  mWorldCreator(&worldCreator), 
		  mLastCreation(0), mTimeCounter(0), mRandomTime(0) {}

	typedef std::list<FieldUnit*> UnitsList;

	// ���������� ������ ������ ��� ���������� �� �����
	UnitsList createRandomUnits(int timeDelta);
};

// �������� �� ������� �����/������
class WorldProcessor {
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

	// ����������� ���������� �� �������
	virtual void updateKilled(float type);
	virtual void updateAlive(float type);

	// ���������� std::map ������
	virtual void initStatistics();

	typedef std::pair<float, float> StepPoint;

	// ���������� ��������� ���������� �����
	virtual StepPoint predictNextEnemyStep(FieldUnit * unit, int time_delta, bool &updateWaypoint);
};

#endif
