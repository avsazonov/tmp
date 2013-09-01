#ifndef WORLDCREATOR_H_INCLUDED
#define WORLDCREATOR_H_INCLUDED

#include <map>
#include <vector>

class BattleField;
#include "FieldUnit.h"

// �����, ������� �������� �������� �� ��������
// �������� �������� ���� (� ������������)
class WorldCreator {
	BattleField * mBattleField;
public:
	// ������������� ������� �����
	WorldCreator(BattleField & battleField);

	// ������� ��������� ������� �� ������� ����
	virtual void create();

	// ���������� ���������� ������
	virtual float getCellsX() const;
	virtual float getCellsY() const;

	typedef enum {
		TOWER_1, TOWER_2, TOWER_3
	} TOWERTYPE;

	typedef enum {
		ENEMY_1, ENEMY_2, ENEMY_3
	} ENEMYTYPE;

	// ������� ��������� �������
	virtual TowerDefense::Tower * createTower(TOWERTYPE towerType) const;
	virtual TowerDefense::Enemy * createEnemy(ENEMYTYPE enemyType) const;
	virtual TowerDefense::Shot * createShot() const;

	// first - x, second - y
	typedef std::pair<float, float> WayPoint;
	typedef std::vector<WayPoint>   PointsList;

	// ���������� ������ ������ �� �����
	virtual const PointsList& getMapEntries() const;

	// ���������� ����� ���������� ����� ��� ��������� �����
	virtual WayPoint getWayPoint(WayPoint entry, int wayPointNumber);

protected:
	PointsList mMapEntries;
	
	typedef std::vector<PointsList> Paths;
	Paths mMapPaths;
};

#endif