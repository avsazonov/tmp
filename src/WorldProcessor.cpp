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

inline float square(float x) { return x*x; }

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

// �������� ��� x, y ����� x1, y1 � x2, y2 (� ��������������)
inline bool checkInBounds(float x1, float y1, float x, float y, float x2, float y2) {
	return (min(x, max(x1, x2)) == x) && (min(y, max(y1, y2)) == y) && (max(x, min(x1, x2)) == x) && (max(y, min(y1, y2)) == y);
}

WorldProcessor::StepPoint WorldProcessor::predictNextEnemyStep(FieldUnit * enemy, int timeDelta, bool &updateWaypoint) {

	updateWaypoint = false;

	// �������� ��������� ����� ���������� �� �����
	WorldCreator::WayPoint entry_point, next_point, current_point;
	entry_point.first = enemy->getSetting("entry_x").getValue();
	entry_point.second = enemy->getSetting("entry_y").getValue();
	int way_point_number = int(enemy->getSetting("way_point_number").getValue());
	
	current_point = mWorldCreator->getWayPoint(entry_point, way_point_number);
	next_point = mWorldCreator->getWayPoint(entry_point, way_point_number + 1);

	StepPoint point;
	point.first = enemy->getSetting("x").getValue();
	point.second = enemy->getSetting("y").getValue();

	// ���� ��������� - ��� ����� - �� ���������� �������
	if ((next_point.first == entry_point.first) && (next_point.second == entry_point.second))
		return point;

	// ��������� ���
	float delta_x = (next_point.first - current_point.first) * 
		enemy->getSetting("speed").getValue() / (1000 / timeDelta);  
	float delta_y = (next_point.second - current_point.second) *
		enemy->getSetting("speed").getValue() / (1000 / timeDelta);  

	// ���� ������� �� ������� �������� ��������������, ���� ��� ��������
	if (!checkInBounds(next_point.first, next_point.second, 
		            point.first + delta_x, point.second + delta_y, 
					current_point.first, current_point.second)) {
		updateWaypoint = true;

		// ��� ���������� ���������
		// ������ (x + delta_x, y + delta_y) ����� �� ������� �������� ��������������, ������������� ������������ �������
		// ���� ����, ��� ���� �������� ����������� �����, ��� ���� � ����������������� ������
		// ����� ������� (delta_x, delta_y) �������������� ������ �� ������� ������������� �������������
		// ���� �� �����, ������ ������������� ���� ������ - ����� ��������� ����������� �����
		// �������� � ����������� ������� - ����, ���� ��������� ��� ����� ��������� ����������� �����

		float move_distance = sqrtf(square(delta_x) + square(delta_y));
		float move_distance_new = sqrtf(square(next_point.first - point.first - delta_x) + square(next_point.second - point.second - delta_y));
		point = next_point; current_point = next_point;
		next_point = mWorldCreator->getWayPoint(entry_point, way_point_number + 2);
		delta_x =  (next_point.first - current_point.first) * 
					enemy->getSetting("speed").getValue() / (1000 / timeDelta) *  
					move_distance_new / move_distance;
		delta_y =  (next_point.second - current_point.second) *
					enemy->getSetting("speed").getValue() / (1000 / timeDelta) *  
					move_distance_new / move_distance;

		// ���� ����� �� ���������� ��������� ��������, �������� ���� ������ ������
	}

	point.first += delta_x;
	point.second += delta_y;

	return point;
}

inline bool checkUnitOut(FieldUnit * unit, float cellsX, float cellsY) {
	// �������� �� ����������� ��� ������ ���� �� ������� ����
	return (unit->getSetting("x").getValue() <= -unit->getSetting("size_x").getValue()) ||
		   (unit->getSetting("y").getValue() <= -unit->getSetting("size_y").getValue()) ||
		   (unit->getSetting("x").getValue() >= cellsX + unit->getSetting("size_x").getValue()) ||
		   (unit->getSetting("y").getValue() >= cellsY + unit->getSetting("size_y").getValue()); 
}

void WorldProcessor::moveEnemy(FieldUnit * unit, int timeDelta) {
		// ������� �����
		bool update_waypoint = false;
		StepPoint next_enemy_step = predictNextEnemyStep(unit, timeDelta, update_waypoint);
		unit->setSetting("x", next_enemy_step.first);
		unit->setSetting("y", next_enemy_step.second);

		// ��������� �������� �������� - �� ������ ��������� ����������� �����
		// ��� predict �� ��������� ��-������ ������ ��� �� predict, � ��-������
		// �� ������������ ��� ��� ���������� ���� ��������
		if (update_waypoint)
			unit->setSetting("way_point_number", unit->getSetting("way_point_number").getValue() + 1);
}

void WorldProcessor::processEnemies(int timeDelta) {
	BattleField::UnitsOnLayer to_delete;

	// �������� �� ���� ������
	for (BattleField::UnitsOnLayer::iterator unit_iterator = (mBattleField->getUnitsOnLayer(TowerDefense::gEnemyLayer)).begin(); 
			unit_iterator != (mBattleField->getUnitsOnLayer(TowerDefense::gEnemyLayer)).end(); 
			++unit_iterator) {
		FieldUnit * unit = *unit_iterator;

		moveEnemy(unit, timeDelta);

		// ���� ���� ����� �� ����� - ������ ������, 
		// � �� ��� �������� �� ��������
		if (checkUnitOut(unit, mWorldCreator->getCellsX(), mWorldCreator->getCellsY())) { 
			to_delete.insert(unit);
			updateAlive(unit->getSetting("type").getValue());
		} else
		// ���� �� �����, �� ����� �� �������� - ������ ����� �����, 
		// � �� ��� ����� �������� �� ��������
		if (unit->getSetting("current_HP").getValue() <= 0) {
			to_delete.insert(unit);
			updateKilled(unit->getSetting("type").getValue());
		}
	}

	// ������� ���������� � ���� �����
	for (BattleField::UnitsOnLayer::iterator unit_iterator = to_delete.begin(); 
		unit_iterator != to_delete.end(); 
		++unit_iterator)
		mBattleField->delUnit(*unit_iterator);
}

void WorldProcessor::moveShot(FieldUnit * shot, int timeDelta) {
	float delta_x = (shot->getSetting("target_x").getValue() - shot->getSetting("tower_x").getValue()) / (gShotLife / timeDelta); 
	float delta_y = (shot->getSetting("target_y").getValue() - shot->getSetting("tower_y").getValue()) / (gShotLife / timeDelta);

	shot->setSetting("x", shot->getSetting("x").getValue() + delta_x);
	shot->setSetting("y", shot->getSetting("y").getValue() + delta_y);
}

bool WorldProcessor::doOneShot(FieldUnit * shot, int timeDelta) {
	// ���� "������", � ������ ������ �������� �� ������� �������������� "���������� ����-�����"
			if (!checkInBounds(shot->getSetting("target_x").getValue(), shot->getSetting("target_y").getValue(),
							   shot->getSetting("x").getValue(), shot->getSetting("y").getValue(),
							   shot->getSetting("tower_x").getValue(), shot->getSetting("tower_y").getValue())) {
				FieldUnit * unit = mBattleField->getConnectedUnit(shot);
				if (unit) {
					// ��������� ����� ���������� � ��������� ����� �� ������ �����
					unit->setSetting("current_HP", unit->getSetting("current_HP").getValue() - shot->getSetting("damage").getValue());
					mBattleField->delConnection(shot);

					// ���� ����� - �������
					if (unit->getSetting("current_HP").getValue() <= 0) {
						updateKilled(unit->getSetting("type").getValue());
						mBattleField->delUnit(unit);
					}
				}
				
				return true;
			}
			// ����� ��� �� ��������
			else {
				// ���������� �������
				moveShot(shot, timeDelta);
			}

			return false;
}

void WorldProcessor::processShots(int timeDelta) {

	BattleField::UnitsOnLayer to_delete;

	// �������� �� ���� ���������
	for (BattleField::UnitsOnLayer::iterator shot_iterator = (mBattleField->getUnitsOnLayer(TowerDefense::gShotLayer)).begin();
		shot_iterator != (mBattleField->getUnitsOnLayer(TowerDefense::gShotLayer)).end();
		++shot_iterator) {
			FieldUnit * shot = *shot_iterator;
		if (doOneShot(shot, timeDelta))
			to_delete.insert(shot);
	}

	for (BattleField::UnitsOnLayer::iterator unit_iterator = to_delete.begin(); unit_iterator != to_delete.end(); ++unit_iterator)
		mBattleField->delUnit(*unit_iterator);
}

void WorldProcessor::doTowerShot(FieldUnit * tower, const BattleField::UnitsOnLayer &enemies, int timeDelta) {
	// ��������� ������� � ���������� �������� ������ �� ������, ��� ����� ���������
		if ((float(mTimeCounter) - tower->getSetting("last_shot").getValue()) < 
		    ((tower)->getSetting("attack_speed").getValue() * 1000))
			// ��������, ���������� �����
			return;

		// �� ��������, ������ ���� ����� �� ���� ��������, �� ���� � ������� ����������� (� ����������)
		for (BattleField::UnitsOnLayer::iterator unit_iterator = enemies.begin();
			unit_iterator != enemies.end();
			++unit_iterator) {

				FieldUnit * unit = *unit_iterator;

				// ���������� ������ ������
				if (unit->getSetting("current_HP").getValue() <= 0)
					continue;
				
				// ��������� ����� - � ����� ����, ����� ��������� ���-����� �� ��������
				// � ������ ����� �����
				float tower_x = tower->getSetting("x").getValue() + 0.5f;
				float tower_y = tower->getSetting("y").getValue() + 0.3f;
				float unit_x  = unit->getSetting("x").getValue();
				float unit_y  = unit->getSetting("y").getValue();

				// ��������� ���������� �� �����
				if ((square(tower_x - unit_x) + square(unit_y - tower_y)) < square(tower->getSetting("radius").getValue())) {
					// ���� ������ ��������� ��� ����� - �� ������� �������
					TowerDefense::Shot * shot = mWorldCreator->createShot();
					if (!shot)
						continue;
					
					shot->setSetting("damage",  tower->getSetting("damage").getValue());
					shot->setSetting("type",    tower->getSetting("type").getValue());

					// ������������� ��������� x � y
					shot->setSetting("tower_x", tower_x);
					shot->setSetting("tower_y", tower_y);
					
					shot->setSetting("x", tower_x);
					shot->setSetting("y", tower_y);
					
					// �������������, ��� �������� ����
					// �� ���� �������� � �����������
					StepPoint target; bool do_update = false;
					// ����� ���� �� �������� ��� ���:
					// target = predictNextEnemyStep(unit, gShotLife, do_update);
					// �� ��� ����� �������� ������ ��� ��������� ��������� ������������
					// ����� ���������� ������ � ������ �������. ������� ������� �������
					
					// �������� ���������
					float current_way_point = unit->getSetting("way_point_number").getValue();

					// �������� �������������, ��� �������� ���� �� ����� ������ �������
					for (int i = 0; i < gShotLife/timeDelta; ++i) {
						target = predictNextEnemyStep(unit, timeDelta, do_update);
						unit->setSetting("x", target.first);
						unit->setSetting("y", target.second);
						if (do_update)
							unit->setSetting("way_point_number", unit->getSetting("way_point_number").getValue() + 1);
					}

					// ������ ��������� �� �����
					unit->setSetting("way_point_number", current_way_point);
					unit->setSetting("x", unit_x); unit->setSetting("y", unit_y);

					// ����� ���������
					shot->setSetting("target_x", target.first + unit->getSetting("size_x").getValue()/2);
					shot->setSetting("target_y", target.second + unit->getSetting("size_y").getValue()/2);

					mBattleField->addShot(shot);
					mBattleField->addConnection(shot, unit);
					tower->setSetting("last_shot", float(mTimeCounter));

					// ���������� - ������� �� ����� �� ������
					break;
				}
		}
}

void WorldProcessor::processTowers(int timeDelta) {
	
	// �������� �� ���� ������
	for (BattleField::UnitsOnLayer::iterator tower_iterator = (mBattleField->getUnitsOnLayer(TowerDefense::gTowerLayer)).begin();
	tower_iterator != (mBattleField->getUnitsOnLayer(TowerDefense::gTowerLayer)).end();
	++tower_iterator) {

		FieldUnit * tower = *tower_iterator;
		
		// ����� ��������� ����, ����������
		if (tower->getObjectName().compare("Tower") != 0)
			continue;
		
		doTowerShot(tower, mBattleField->getUnitsOnLayer(TowerDefense::gEnemyLayer), timeDelta);
		
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
