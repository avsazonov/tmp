#ifndef WORLDCREATOR_H_INCLUDED
#define WORLDCREATOR_H_INCLUDED

#include <map>
#include <vector>

class BattleField;
#include "FieldUnit.h"

// Класс, объекты которого отвечают за создание
// объектов игрового мира (в совокупности)
class WorldCreator {
	BattleField * mBattleField;
public:
	// инициализация игровым полем
	WorldCreator(BattleField & battleField);

	// создает начальные объекты на игровом поле
	virtual void create();

	// возвращают количество клеток
	virtual float getCellsX() const;
	virtual float getCellsY() const;

	typedef enum {
		TOWER_1, TOWER_2, TOWER_3
	} TOWERTYPE;

	typedef enum {
		ENEMY_1, ENEMY_2, ENEMY_3
	} ENEMYTYPE;

	// создают указанные объекты
	virtual TowerDefense::Tower * createTower(TOWERTYPE towerType) const;
	virtual TowerDefense::Enemy * createEnemy(ENEMYTYPE enemyType) const;
	virtual TowerDefense::Shot * createShot() const;

	// first - x, second - y
	typedef std::pair<float, float> WayPoint;
	typedef std::vector<WayPoint>   PointsList;

	// возвращает список входов на карту
	virtual const PointsList& getMapEntries() const;

	// возвращает точку следования врага для заданного входа
	virtual WayPoint getWayPoint(WayPoint entry, int wayPointNumber);

protected:
	PointsList mMapEntries;
	
	typedef std::vector<PointsList> Paths;
	Paths mMapPaths;
};

#endif