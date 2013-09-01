#ifndef WORLDPROCESSOR_H_INCLUDED
#define WORLDPROCESSOR_H_INCLUDED

#include <map>
#include <list>

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

#endif
