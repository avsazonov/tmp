#ifndef WORLDPROCESSOR_H_INCLUDED
#define WORLDPROCESSOR_H_INCLUDED

#include <map>
#include <list>

class WorldCreator;
class FieldUnit;
class BattleField;

// ќбъекты отвечают за генерацию 
// новых врагов
class Randomizer {
	float mLastCreation, mTimeCounter, mRandomTime;
	WorldCreator * mWorldCreator;
public:
	Randomizer(WorldCreator & worldCreator) :
	  mWorldCreator(&worldCreator), 
		  mLastCreation(0), mTimeCounter(0), mRandomTime(0) {}

	typedef std::list<FieldUnit*> UnitsList;

	// возвращает список врагов дл€ добавлени€ на карту
	UnitsList createRandomUnits(int timeDelta);
};

// ќтвечает за процесс атаки/защиты
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

	// сделать шаг игрового мира
	virtual void   generateStep(int timeDelta);
	
	// прошедшее врем€, в секундах
	virtual float  getProcessTime() const;
	
	typedef std::map<float, int> StatisticsMap;

private:
	StatisticsMap mKilledStatistics, mAliveStatistics;

public:

	// вз€ть статистику, тип <--> количество (убитых, убежавших)
	virtual StatisticsMap& getKilledStatistics() { return mKilledStatistics; }
	virtual StatisticsMap& getAliveStatistics()  { return mAliveStatistics;  }

protected:
	// обработчики. удал€ют лишние объекты с карты
	virtual void processEnemies(int timeDelta);
	virtual void processShots(int timeDelta);
	virtual void processTowers(int timeDelta);

	// увеличивают статистику на единицу
	virtual void updateKilled(float type);
	virtual void updateAlive(float type);

	// заполнение std::map нул€ми
	virtual void initStatistics();

	typedef std::pair<float, float> StepPoint;

	// вычисление следующей координаты врага
	virtual StepPoint predictNextEnemyStep(FieldUnit * unit, int time_delta, bool &updateWaypoint);
};

#endif
