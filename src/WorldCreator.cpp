#include "FieldUnit.h"
#include "WorldCreator.h"

WorldCreator::WorldCreator(BattleField& battleField) : mBattleField(&battleField) {}

using namespace TowerDefense;

// условимся на том, что у нас поле 20x11
float WorldCreator::getCellsX() const {
	return 20.f;
}

// условимся на том, что у нас поле 20x11
float WorldCreator::getCellsY() const {
	return 11.f;
}

const WorldCreator::PointsList& WorldCreator::getMapEntries() const {
	return mMapEntries;
}

void WorldCreator::create() {
	// создание первоначальных объектов

	mBattleField->addBackGroundCell(new BackGroundCell(0.f, 0.f, getCellsX(), getCellsY()));

	// Входы врагов на карту
	mMapEntries.push_back(WayPoint(getCellsX(), 3.f));
	mMapEntries.push_back(WayPoint(getCellsX(), 7.f));

	PointsList first_way, second_way;

	for (int x = 0; x < 20; ++x) {
		// добавляем дорожки. их можно сделать кривыми, это будет работать
		mBattleField->addPathCell(new PathCell(float(x), 3.f));
		first_way.push_back(WayPoint(getCellsX() - float(x), 3.f));
		mBattleField->addPathCell(new PathCell(float(x), 7.f));
		second_way.push_back(WayPoint(getCellsX() - float(x), 7.f));

		// каждая пятая клетка начиная с третьей - слот для башни
		if (0 == (x + 3) % 5) {
			mBattleField->addTowerSlot(new TowerSlot(float(x), 2.f));
			mBattleField->addTowerSlot(new TowerSlot(float(x), 4.f));
			mBattleField->addTowerSlot(new TowerSlot(float(x), 6.f));
			mBattleField->addTowerSlot(new TowerSlot(float(x), 8.f));
		} 
	}

	// добавляем недостающие путевые точки
	first_way.push_back(WayPoint(0.f, 3.f));
	second_way.push_back(WayPoint(0.f, 7.f));

	first_way.push_back(WayPoint(-1.f, 3.f));
	second_way.push_back(WayPoint(-1.f, 7.f));

	// создаем пути в объекте
	mMapPaths.push_back(first_way);
	mMapPaths.push_back(second_way);
}

TowerDefense::Tower * WorldCreator::createTower(WorldCreator::TOWERTYPE towerType) const {
	// создание башни в зависимости от запрошенного типа
	Tower * tower = 0;
	if (TOWER_1 == towerType) 
		tower = new Tower(1, 20, 4, towerType + 1, this);
	else if (TOWER_2 == towerType)
		tower = new Tower(2, 50, 3, towerType + 1, this);
	else if (TOWER_3 == towerType) 
		tower = new Tower(4, 125, 2, towerType + 1, this);
	return tower;
};

TowerDefense::Enemy * WorldCreator::createEnemy(WorldCreator::ENEMYTYPE enemyType) const {
	// создание врага в зависимости от запрошенного типа
	Enemy * enemy = 0;
	if (ENEMY_1 == enemyType) 
		enemy = new Enemy(50, 1.f, enemyType + 1, this);
	else if (ENEMY_2 == enemyType)
		enemy = new Enemy(100, 0.5f, enemyType + 1, this);
	else if (ENEMY_3 == enemyType)
		enemy = new Enemy(200, 0.25f, enemyType + 1, this);
	return enemy;
};

TowerDefense::Shot * WorldCreator::createShot() const {
	// создание выстрела
	return new Shot;
}

WorldCreator::WayPoint WorldCreator::getWayPoint(WayPoint entry, int wayPointNumber) const {

	// если ноль, то это вход
	if (0 == wayPointNumber)
		return entry;

	Paths::const_iterator path_iterator;

	// Ищем путь, соотвествующий входу
	for (path_iterator = mMapPaths.cbegin();
		path_iterator != mMapPaths.cend();
		++path_iterator) 
		if ((path_iterator->at(0).first == entry.first) && (path_iterator->at(0).second == entry.second))
			break;

	// Не нашли - возвращаем исходную точку
	if (mMapPaths.end() == path_iterator)
		return entry;

	if (int(path_iterator->size()) > wayPointNumber)
		return path_iterator->at(wayPointNumber);
	else
		// запросили больше чем есть - возвращаем исходную
		return entry;
}
