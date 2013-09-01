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

// проверка что x, y между x1, y1 и x2, y2 (в прямоугольнике)
inline bool checkInBounds(float x1, float y1, float x, float y, float x2, float y2) {
	return (min(x, max(x1, x2)) == x) && (min(y, max(y1, y2)) == y) && (max(x, min(x1, x2)) == x) && (max(y, min(y1, y2)) == y);
}

WorldProcessor::StepPoint WorldProcessor::predictNextEnemyStep(TowerDefense::Enemy * enemy, int timeDelta, bool &updateWaypoint) {

	updateWaypoint = false;

	// Получаем следующую точку следования по карте
	WorldCreator::WayPoint entry_point, next_point, current_point;
	entry_point.first = enemy->getSetting("entry_x").getValue();
	entry_point.second = enemy->getSetting("entry_y").getValue();
	int way_point_number = int(enemy->getSetting("way_point_number").getValue());
	
	current_point = mWorldCreator->getWayPoint(entry_point, way_point_number);
	next_point = mWorldCreator->getWayPoint(entry_point, way_point_number + 1);

	StepPoint point;
	point.first = enemy->getSetting("x").getValue();
	point.second = enemy->getSetting("y").getValue();

	// Если следующая - это старт - то возвращаем текущую
	if ((next_point.first == entry_point.first) && (next_point.second == entry_point.second))
		return point;

	// вычисляем ход
	float delta_x = (next_point.first - current_point.first) * 
		enemy->getSetting("speed").getValue() / (1000 / timeDelta);  
	float delta_y = (next_point.second - current_point.second) *
		enemy->getSetting("speed").getValue() / (1000 / timeDelta);  

	// если выходим за пределы текущего прямоугольника, надо все обновить
	if (!checkInBounds(next_point.first, next_point.second, 
		            point.first + delta_x, point.second + delta_y, 
					current_point.first, current_point.second)) {
		updateWaypoint = true;

		// тут происходит следующее
		// вектор (x + delta_x, y + delta_y) вышел за пределы текущего прямоугольника, ограниченного контрольными точками
		// мало того, что надо обновить контрольную точку, так надо и подкорректировать вектор
		// длина вектора (delta_x, delta_y) корректируется исходя из подобия прямоугольных треугольников
		// сама же точка, откуда откладывается этот вектор - точка следующей контрольной точки
		// меняется и направление вектора - туда, куда указывает еще более следующая контрольная точка

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

		// этим самым мы добиваемся плавности движения, особенно если дорога кривая
	}

	point.first += delta_x;
	point.second += delta_y;

	return point;
}

inline bool checkUnitOut(FieldUnit * unit, float cellsX, float cellsY) {
	// проверка по координатам что объект ушел за пределы поля
	return (unit->getSetting("x").getValue() <= -unit->getSetting("size_x").getValue()) ||
		   (unit->getSetting("y").getValue() <= -unit->getSetting("size_y").getValue()) ||
		   (unit->getSetting("x").getValue() >= cellsX + unit->getSetting("size_x").getValue()) ||
		   (unit->getSetting("y").getValue() >= cellsY + unit->getSetting("size_y").getValue()); 
}

void WorldProcessor::moveEnemy(TowerDefense::Enemy * unit, int timeDelta) {
		// двигаем врага
		bool update_waypoint = false;
		StepPoint next_enemy_step = predictNextEnemyStep(unit, timeDelta, update_waypoint);
		unit->setSetting("x", next_enemy_step.first);
		unit->setSetting("y", next_enemy_step.second);

		// требуется обновить значения - мы прошли некоторую контрольную точку
		// сам predict не обновляет во-первых потому что он predict, а во-вторых
		// он используется еще для вычисления цели выстрела
		if (update_waypoint)
			unit->setSetting("way_point_number", unit->getSetting("way_point_number").getValue() + 1);
}

void WorldProcessor::processEnemies(int timeDelta) {
	BattleField::EnemiesSet to_delete;

	// проходим по всем врагам
	for (BattleField::EnemiesSet::iterator unit_iterator = (mBattleField->getEnemies()).begin(); 
			unit_iterator != (mBattleField->getEnemies()).end(); 
			++unit_iterator) {
		TowerDefense::Enemy * unit = *unit_iterator;

		moveEnemy(unit, timeDelta);

		// если враг уехал за экран - значит сбежал, 
		// и мы его помечаем на удаление
		if (checkUnitOut(unit, mWorldCreator->getCellsX(), mWorldCreator->getCellsY())) { 
			to_delete.insert(unit);
			updateAlive(unit->getSetting("type").getValue());
		} else
		// если не уехал, но жизни не осталось - значит врага убили, 
		// и мы его также помечаем на удаление
		if (unit->getSetting("current_HP").getValue() <= 0) {
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

void WorldProcessor::moveShot(TowerDefense::Shot * shot, int timeDelta) {
	float delta_x = (shot->getSetting("target_x").getValue() - shot->getSetting("tower_x").getValue()) / (gShotLife / timeDelta); 
	float delta_y = (shot->getSetting("target_y").getValue() - shot->getSetting("tower_y").getValue()) / (gShotLife / timeDelta);

	shot->setSetting("x", shot->getSetting("x").getValue() + delta_x);
	shot->setSetting("y", shot->getSetting("y").getValue() + delta_y);
}

bool WorldProcessor::doOneShot(TowerDefense::Shot * shot, int timeDelta) {
	// если "попали", в данном случае вылетели за пределы прямоугольника "координаты цели-башня"
			if (!checkInBounds(shot->getSetting("target_x").getValue(), shot->getSetting("target_y").getValue(),
							   shot->getSetting("x").getValue(), shot->getSetting("y").getValue(),
							   shot->getSetting("tower_x").getValue(), shot->getSetting("tower_y").getValue())) {
				TowerDefense::Enemy * unit = mBattleField->getConnectedUnit(shot);
				if (unit) {
					// уменьшаем жизнь связанного с выстрелом врага на размер урона
					unit->setSetting("current_HP", unit->getSetting("current_HP").getValue() - shot->getSetting("damage").getValue());
					mBattleField->delConnection(shot);

					// если убили - удаляем
					if (unit->getSetting("current_HP").getValue() <= 0) {
						updateKilled(unit->getSetting("type").getValue());
						mBattleField->delEnemy(unit);
					}
				}
				
				return true;
			}
			// иначе еще не долетели
			else {
				// перемещаем выстрел
				moveShot(shot, timeDelta);
			}

			return false;
}

void WorldProcessor::processShots(int timeDelta) {

	BattleField::ShotsSet to_delete;

	// проходим по всем выстрелам
	for (BattleField::ShotsSet::iterator shot_iterator = (mBattleField->getShots()).begin();
		shot_iterator != (mBattleField->getShots()).end();
		++shot_iterator) {
		TowerDefense::Shot * shot = *shot_iterator;
		if (doOneShot(shot, timeDelta))
			to_delete.insert(shot);
	}

	for (BattleField::ShotsSet::iterator unit_iterator = to_delete.begin(); unit_iterator != to_delete.end(); ++unit_iterator)
		mBattleField->delShot(*unit_iterator);
}

void WorldProcessor::doTowerShot(TowerDefense::Tower * tower, const BattleField::EnemiesSet &enemies, int timeDelta) {
	// проверяем времени с последнего выстрела больше ли прошло, чем башня позволяет
		if ((float(mTimeCounter) - tower->getSetting("last_shot").getValue()) < 
		    ((tower)->getSetting("attack_speed").getValue() * 1000))
			// стреляли, пропускаем башню
			return;

		// не стреляли, значит надо найти по кому стрелять, то есть в первого попавшегося (в контейнере)
		for (BattleField::EnemiesSet::iterator unit_iterator = enemies.begin();
			unit_iterator != enemies.end();
			++unit_iterator) {

				TowerDefense::Enemy * unit = *unit_iterator;

				// пропускаем убитых врагов
				if (unit->getSetting("current_HP").getValue() <= 0)
					continue;
				
				// небольшой сдвиг - с целью того, чтобы выглядело как-будто мы стреляем
				// с центра крыши башни
				float tower_x = tower->getSetting("x").getValue() + 0.5f;
				float tower_y = tower->getSetting("y").getValue() + 0.3f;
				float unit_x  = unit->getSetting("x").getValue();
				float unit_y  = unit->getSetting("y").getValue();

				// проверяем расстояние до юнита
				if ((square(tower_x - unit_x) + square(unit_y - tower_y)) < square(tower->getSetting("radius").getValue())) {
					// если меньше дистанции для башни - то создаем выстрел
					TowerDefense::Shot * shot = mWorldCreator->createShot();
					if (!shot)
						continue;
					
					shot->setSetting("damage",  tower->getSetting("damage").getValue());
					shot->setSetting("type",    tower->getSetting("type").getValue());

					// устанавливаем начальные x и y
					shot->setSetting("tower_x", tower_x);
					shot->setSetting("tower_y", tower_y);
					
					shot->setSetting("x", tower_x);
					shot->setSetting("y", tower_y);
					
					// предсказываем, где окажется юнит
					// то есть стреляем с упреждением
					StepPoint target; bool do_update = false;
					// можно было бы написать вот так:
					// target = predictNextEnemyStep(unit, gShotLife, do_update);
					// но это будет работать только для некоторых небольших соответствий
					// между скоростями юнитов и полета снаряда. Поэтому сделаем сложнее
					
					// запомним настройки
					float current_way_point = unit->getSetting("way_point_number").getValue();

					// пошагово предсказываем, где окажется враг за время полета снаряда
					for (int i = 0; i < gShotLife/timeDelta; ++i) {
						target = predictNextEnemyStep(unit, timeDelta, do_update);
						unit->setSetting("x", target.first);
						unit->setSetting("y", target.second);
						if (do_update)
							unit->setSetting("way_point_number", unit->getSetting("way_point_number").getValue() + 1);
					}

					// вернем настройки на место
					unit->setSetting("way_point_number", current_way_point);
					unit->setSetting("x", unit_x); unit->setSetting("y", unit_y);

					// место попадания
					shot->setSetting("target_x", target.first + unit->getSetting("size_x").getValue()/2);
					shot->setSetting("target_y", target.second + unit->getSetting("size_y").getValue()/2);

					mBattleField->addShot(shot);
					mBattleField->addConnection(shot, unit);
					tower->setSetting("last_shot", float(mTimeCounter));

					// выстрелили - выходим из цикла по юнитам
					break;
				}
		}
}

void WorldProcessor::processTowers(int timeDelta) {
	
	// проходим по всем башням
	for (BattleField::TowersSet::iterator tower_iterator = (mBattleField->getTowers()).begin();
	tower_iterator != (mBattleField->getTowers()).end();
	++tower_iterator) {

		TowerDefense::Tower * tower = *tower_iterator;
		
		// может попасться слот, пропускаем
		if (tower->getObjectName().compare("Tower") != 0)
			continue;
		
		doTowerShot(tower, mBattleField->getEnemies(), timeDelta);
		
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
