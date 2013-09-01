#include "WorldCreator.h"
#include "WorldProcessor.h"
#include "FieldUnit.h"
#include "LayerConstants.h"

#include <cmath>
#include <cstdlib>

Randomizer::UnitsList Randomizer::createRandomUnits(int timeDelta) {

	mTimeCounter += float(timeDelta);

	// готовим под возвращаемое значение пустой контейнер
	Randomizer::UnitsList result;

	if ((mTimeCounter - mLastCreation) > mRandomTime) {

		// Проходим по всем входам на карту и создаем врага случайного типа
		for (WorldCreator::PointsList::const_iterator entry_iterator = mWorldCreator->getMapEntries().begin();
			entry_iterator != mWorldCreator->getMapEntries().end();
			++entry_iterator) {

			int enemy_type = rand() % 3;

			// Создаем врага
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

		// вычисляем случайное время ожидания
		mRandomTime =
			// от 1 до 1.5 секунд
			(1.f + float(rand())/RAND_MAX/2) * 1000;
	}

	// ничего страшного
	// если пара указателей будет скопирована
	// раз в секунду или того реже
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
// проверка что x, y между x1, y1 и x2, y2 (в прямоугольнике)
inline bool checkInBounds(float x1, float y1, float x, float y, float x2, float y2) {
	return (min(x, max(x1, x2)) == x) && (min(y, max(y1, y2)) == y) && (max(x, min(x1, x2)) == x) && (max(y, min(y1, y2)) == y);
}

// TODO: move to utilities.h
inline float square(float x) { return x*x; }

void WorldProcessor::processEnemies(int timeDelta) {
	BattleField::EnemiesSet to_delete;

	// проходим по всем врагам
	for (BattleField::EnemiesSet::iterator unit_iterator = (mBattleField->getEnemies()).begin(); 
			unit_iterator != (mBattleField->getEnemies()).end(); 
			++unit_iterator) {
		TowerDefense::Enemy * unit = *unit_iterator;

		unit->move(timeDelta);

		// если враг уехал за экран - значит сбежал, 
		// и мы его помечаем на удаление
		if (unit->runOut()) { 
			to_delete.insert(unit);
			updateAlive(unit->getSetting("type").getValue());
		} else
		// если не уехал, но жизни не осталось - значит врага убили, 
		// и мы его также помечаем на удаление
		if (unit->isDestroyed()) {
			to_delete.insert(unit);
			updateKilled(unit->getSetting("type").getValue());
		}
	}

	// удаляем помеченные с поля битвы
	for (BattleField::EnemiesSet::iterator unit_iterator = to_delete.begin(); 
		unit_iterator != to_delete.end(); 
		++unit_iterator)
		mBattleField->delEnemy(*unit_iterator);
}

void WorldProcessor::processShots(int timeDelta) {

	BattleField::ShotsSet to_delete;

	// проходим по всем выстрелам
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
	
	// проходим по всем башням
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
		// три типа врагов (1, 2, 3)
		for (int i = 1; i < 4; ++i) {
			mAliveStatistics[float(i)] = 0;
			mKilledStatistics[float(i)] = 0;
		}
		mStatisticsInited = true;
	}
}

void WorldProcessor::generateStep(int timeDelta) {
	
	// чтобы каждый раз не делать поиск по std::map статистик
	// для инициализации значений по типам врагов
	// гарантируем, что они заполнены
	initStatistics();

	mTimeCounter += float(timeDelta);

	// добавляем случайных врагов на карту
	Randomizer::UnitsList to_add(mRandomizer.createRandomUnits(timeDelta));
	for (Randomizer::UnitsList::iterator unit_iterator = to_add.begin();
		unit_iterator != to_add.end();
		++unit_iterator)
		mBattleField->addEnemy(*unit_iterator);

	// обрабатываем передвижение и жизнь врагов
	processEnemies(timeDelta);

	// обрабатываем передвижение и жизнь выстрелов
	processShots(timeDelta);

	// обрабатываем башни и генерируем выстрелы
	processTowers(timeDelta);

}
