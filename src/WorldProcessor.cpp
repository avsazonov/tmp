#include "WorldCreator.h"
#include "WorldProcessor.h"
#include "FieldUnit.h"
#include "LayerConstants.h"

#include <cmath>
#include <cstdlib>

Randomizer::UnitsList Randomizer::createRandomUnits(int timeDelta) {

	mTimeCounter += float(timeDelta);

	// ������� ��� ������������ �������� ������ ���������
	Randomizer::UnitsList result;

	if ((mTimeCounter - mLastCreation) > mRandomTime) {

		// �������� �� ���� ������ �� ����� � ������� ����� ���������� ����
		for (WorldCreator::PointsList::const_iterator entry_iterator = mWorldCreator->getMapEntries().begin();
			entry_iterator != mWorldCreator->getMapEntries().end();
			++entry_iterator) {

			int enemy_type = rand() % 3;

			// ������� �����
			TowerDefense::Enemy * enemy = mWorldCreator->createEnemy(WorldCreator::ENEMYTYPE(enemy_type));
			if (enemy) {
				enemy->setSetting("x", entry_iterator->first);
				enemy->setSetting("y", entry_iterator->second);
				enemy->setSetting("entry_x", entry_iterator->first);
				enemy->setSetting("entry_y", entry_iterator->second);
				enemy->setSetting("way_point_number", 0);
				result.push_back(enemy);
			}
		}

		mLastCreation = mTimeCounter;

		// ��������� ��������� ����� ��������
		mRandomTime =
			// �� 1 �� 1.5 ������
			(1.f + float(rand())/RAND_MAX/2) * 1000;
	}

	// ������ ���������
	// ���� ���� ���������� ����� �����������
	// ��� � ������� ��� ���� ����
	return result;
}

float WorldProcessor::getProcessTime() const {
	return float(mTimeCounter / 1000);
}

void WorldProcessor::updateAlive(float type) {
	++mAliveStatistics[type];
}

void WorldProcessor::updateKilled(float type) {
	++mKilledStatistics[type];
}

inline float max(float a, float b) { return a > b ? a : b; }
inline float min(float a, float b) { return a < b ? a : b; }

// TODO: move to utilities.h
// �������� ��� x, y ����� x1, y1 � x2, y2 (� ��������������)
inline bool checkInBounds(float x1, float y1, float x, float y, float x2, float y2) {
	return (min(x, max(x1, x2)) == x) && (min(y, max(y1, y2)) == y) && (max(x, min(x1, x2)) == x) && (max(y, min(y1, y2)) == y);
}

// TODO: move to utilities.h
inline float square(float x) { return x*x; }

void WorldProcessor::processEnemies(int timeDelta) {
	BattleField::EnemiesSet to_delete;

	// �������� �� ���� ������
	for (BattleField::EnemiesSet::iterator unit_iterator = (mBattleField->getEnemies()).begin(); 
			unit_iterator != (mBattleField->getEnemies()).end(); 
			++unit_iterator) {
		TowerDefense::Enemy * unit = *unit_iterator;

		unit->move(timeDelta);

		// ���� ���� ����� �� ����� - ������ ������, 
		// � �� ��� �������� �� ��������
		if (unit->runOut()) { 
			to_delete.insert(unit);
			updateAlive(unit->getSetting("type").getValue());
		} else
		// ���� �� �����, �� ����� �� �������� - ������ ����� �����, 
		// � �� ��� ����� �������� �� ��������
		if (unit->isDestroyed()) {
			to_delete.insert(unit);
			updateKilled(unit->getSetting("type").getValue());
		}
	}

	// ������� ���������� � ���� �����
	for (BattleField::EnemiesSet::iterator unit_iterator = to_delete.begin(); 
		unit_iterator != to_delete.end(); 
		++unit_iterator)
		mBattleField->delEnemy(*unit_iterator);
}

void WorldProcessor::processShots(int timeDelta) {

	BattleField::ShotsSet to_delete;

	// �������� �� ���� ���������
	for (BattleField::ShotsSet::iterator shot_iterator = (mBattleField->getShots()).begin();
		shot_iterator != (mBattleField->getShots()).end();
		++shot_iterator) {
		
		TowerDefense::Shot * shot = *shot_iterator;
		shot->move(timeDelta);

		if (shot->reachedTarget()) {
			shot->damageTarget();
			to_delete.insert(shot);	
		} 
	}

	for (BattleField::ShotsSet::iterator unit_iterator = to_delete.begin(); unit_iterator != to_delete.end(); ++unit_iterator)
		mBattleField->delShot(*unit_iterator);
}

void WorldProcessor::processTowers(int timeDelta) {
	
	// �������� �� ���� ������
	for (BattleField::TowersSet::iterator tower_iterator = (mBattleField->getTowers()).begin();
	tower_iterator != (mBattleField->getTowers()).end();
	++tower_iterator) {

		TowerDefense::Tower * tower = *tower_iterator;
				
		TowerDefense::Shot * shot = tower->shootEnemy(mBattleField->getEnemies(), mTimeCounter, timeDelta);
		if (0 != shot) 
			mBattleField->addShot(shot);	
	}

}

void WorldProcessor::initStatistics() {
	if (!mStatisticsInited) {
		// ��� ���� ������ (1, 2, 3)
		for (int i = 1; i < 4; ++i) {
			mAliveStatistics[float(i)] = 0;
			mKilledStatistics[float(i)] = 0;
		}
		mStatisticsInited = true;
	}
}

void WorldProcessor::generateStep(int timeDelta) {
	
	// ����� ������ ��� �� ������ ����� �� std::map ���������
	// ��� ������������� �������� �� ����� ������
	// �����������, ��� ��� ���������
	initStatistics();

	mTimeCounter += float(timeDelta);

	// ��������� ��������� ������ �� �����
	Randomizer::UnitsList to_add(mRandomizer.createRandomUnits(timeDelta));
	for (Randomizer::UnitsList::iterator unit_iterator = to_add.begin();
		unit_iterator != to_add.end();
		++unit_iterator)
		mBattleField->addEnemy(*unit_iterator);

	// ������������ ������������ � ����� ������
	processEnemies(timeDelta);

	// ������������ ������������ � ����� ���������
	processShots(timeDelta);

	// ������������ ����� � ���������� ��������
	processTowers(timeDelta);

}
