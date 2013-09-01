#ifndef FIELDUNIT_H_INCLUDED
#define FIELDUNIT_H_INCLUDED

#include "GameObject.h"
#include <set>
#include <list>

// ����������� ����� ��� ���� ������������ �� ���� ��������.
// ������� ������������ ����� �������.
// ������ ������ �� ���� ����� ���������� z ("layer").
// ��� ���������� ������� ��������� (�� �������� � ��������).
// �� ��������� 0, � ������� 0 - �� ��������������.
class FieldUnit : public GameObject, public Renderable {

	std::string mObjectName;

public:
	// ��� ����� - ������� ������ �� �������
	FieldUnit(const std::string& objectName) {
		mObjectName = objectName;

		// ������ ������ �� ��������� layer = 0
		setSetting("layer", Setting(0));
	}

	std::string getObjectName() const {
		return mObjectName;
	}

	// �������������� ����� ���������
	void render() const {
		if (getSetting("layer").isSet() && (getSetting("layer").getValue() > 0.f))
			doRender();
	}

protected:

	// �������� �� �����������
	virtual void doRender() const = 0;
};

namespace TowerDefense {

	// ������� �����. ��������������� �����������������, ������ � ���������� ���������.
	// ��� ���������� ������ ��� �����������
	class Tower : public FieldUnit {
		std::string mResourceName;
	public:
		Tower(float attackSpeed, float damage, float radius, int type);
		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	protected:
		virtual void doRender() const;
	};

	// ������ ������� ����� ��� �����
	class TowerSlot : public FieldUnit {
	public:
		TowerSlot(float x, float y);
		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	protected:
		virtual void doRender() const;
	};

	// ������� ������. ��������������� ��������� ��������� ������� � ��������� ��������.
	// ��� ���������� ������ ��� �����������
	class Enemy : public FieldUnit {
		std::string mResourceName;
	public:
		Enemy(float HP, float speed, int type);

	protected:
		virtual void doRender() const;
	};

	// ������ ����� ������ �������, �� ������� ����� ��������� ����.
	// �������� ���������� ������.
	class PathCell : public FieldUnit {
	public:
		PathCell(float x, float y);

	protected:
		virtual void doRender() const;
	};

	// ������ ����� ������ ���� (�������� ������������ � ��������).
	class BackGroundCell : public FieldUnit {
		std::string mResourceName;
	public:
		BackGroundCell(float x, float y, float size_x, float size_y);

	protected:
		virtual void doRender() const;
	};

	// ������ ��������.
	class Shot : public FieldUnit {
	public:
		Shot();

	protected:
		virtual void doRender() const;
	};
}

// ������� ���� ������� �� ����������� �� ������� �����������
// ������� �������� �� ��.
// ��� ������� ��� �������!
// �� ���� ���� ���� ������ DelUnit, ��� ~BattleField - ��� ����
// �������� ���������� delete. ����� �������, ������ ��������� 
// ������� �� �����, ���� �� ���������� ��������� �� ������� ����.
class BattleField {

public:
	typedef std::set<FieldUnit*>		   UnitsOnLayer;
	typedef std::map<float, UnitsOnLayer>  BattleMap;
	/*
	typedef std::set<TowerDefense::Shot*>			   ShotsSet;
	typedef std::set<TowerDefense::Enemy*>			   EnemiesSet;
	typedef std::set<TowerDefense::Tower*>			   TowersSet;
	typedef std::set<TowerDefense::TowerSlot*>		   TowerSlotsSet;
	typedef std::set<TowerDefense::BackGroundCell*>	   BackGroundCellsSet;
	typedef std::set<TowerDefense::PathCell*>		   PathCellsSet;
	*/
	typedef UnitsOnLayer ShotsSet;
	typedef UnitsOnLayer EnemiesSet;
	typedef UnitsOnLayer TowersSet;
	typedef UnitsOnLayer TowerSlotsSet;
	typedef UnitsOnLayer BackGroundCellsSet;
	typedef UnitsOnLayer PathCellsSet;

private:

	ShotsSet			mShots;
	EnemiesSet			mEnemies;
	TowersSet			mTowers;
	TowerSlotsSet		mTowerSlots;
	BackGroundCellsSet	mBackGroundCells;
	PathCellsSet		mPathCells;

	// ���������� ������� �� �����
	void addUnit(FieldUnit * unit);

public:

	// �������������� ��������
	// � ��� ����� � �� ������
	void delUnit(FieldUnit * unit);

	// refactoring: methods definition
	// del methods deletes from memory
	void addShot(TowerDefense::Shot*);
	const ShotsSet& getShots();
	void delShot(TowerDefense::Shot*);
	void addEnemy(TowerDefense::Enemy*);
	const EnemiesSet& getEnemies();
	void delEnemy(TowerDefense::Enemy*);
	void addTower(TowerDefense::Tower*);
	const TowersSet& getTowers();
	void delTower(TowerDefense::Tower*);
	void addTowerSlot(TowerDefense::TowerSlot*);
	const TowerSlotsSet& getTowerSlots();
	void delTowerSlot(TowerDefense::TowerSlot*);
	void addBackGroundCell(TowerDefense::BackGroundCell*);
	const BackGroundCellsSet& getBackGroundCells();
	void delBackGroundCell(TowerDefense::BackGroundCell*);
	void addPathCell(TowerDefense::PathCell*);
	const PathCellsSet& getPathCells();
	void delPathCell(TowerDefense::PathCell*);

	// ���������� ����� ����� ����� ���������
	void addConnection(FieldUnit * firstUnit, FieldUnit * secondUnit);

	// �������� ������ �������
	// ��� ������ �� ���������
	// ��������� ����� �����!
	void delConnection(FieldUnit * unit);

	// ��������� ���������� �������
	// ����� �������, ���� ������ ������ ������,
	// �� ���� ���, ������� ���������, � �� ��
	// ������� ���������. firstUnit �� addConnection
	FieldUnit * getConnectedUnit(FieldUnit * unit);

	// ������ �� �����
	const BattleMap& getClickableUnits();
	const BattleMap& getBattleMap();

	virtual ~BattleField();

private:
	const UnitsOnLayer& getUnitsOnLayer(float layer) {
		return mBattleField[layer];
	}

	typedef std::pair<FieldUnit*, FieldUnit*> ConnectedPair;
	typedef std::list<ConnectedPair>          ConnectionsList;

	ConnectionsList mUnitConnections;
	BattleMap       mBattleField;
	BattleMap       mClickableUnits;
};

#endif