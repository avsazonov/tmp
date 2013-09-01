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

class WorldCreator;

namespace TowerDefense {

	class Enemy;
	class Shot;
	// ������� �����. ��������������� �����������������, ������ � ���������� ���������.
	// ��� ���������� ������ ��� �����������
	class Tower : public FieldUnit {
		std::string mResourceName;
		const WorldCreator * const mWorldCreator;
	public:
		Tower(float attackSpeed, float damage, float radius, int type, const WorldCreator * const creator);
		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
		TowerDefense::Shot * shootEnemy(const std::set<TowerDefense::Enemy*> &enemies, float currentTime, int timeDelta);
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
		const WorldCreator * const mWorldCreator;
	public:
		Enemy(float HP, float speed, int type, const WorldCreator * const);
		std::pair<float, float> predictNextStep(int timeDelta, bool &do_update);
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
		Enemy * mLinkedUnit;
	public:
		Shot();
		
		void addLink(Enemy * linkedUnit) { mLinkedUnit = linkedUnit; }
		Enemy * getLinked() { return mLinkedUnit; }

	protected:
		virtual void doRender() const;
	};
}

namespace tut
{
    template<typename T>
    class test_object;
}

// ������� ���� ������� �� ����������� �� ������� �����������
// ������� �������� �� ��.
// ��� ������� ��� �������!
// �� ���� ���� ���� ������ DelUnit, ��� ~BattleField - ��� ����
// �������� ���������� delete. ����� �������, ������ ��������� 
// ������� �� �����, ���� �� ���������� ��������� �� ������� ����.
class BattleField {

	template<typename T>
	friend class tut::test_object;

public:
	typedef std::set<FieldUnit*>		   UnitsOnLayer;
	typedef std::map<float, UnitsOnLayer>  BattleMap;
	
	typedef std::set<TowerDefense::Shot*>			   ShotsSet;
	typedef std::set<TowerDefense::Enemy*>			   EnemiesSet;
	typedef std::set<TowerDefense::Tower*>			   TowersSet;
	typedef std::set<TowerDefense::TowerSlot*>		   TowerSlotsSet;
	typedef std::set<TowerDefense::BackGroundCell*>	   BackGroundCellsSet;
	typedef std::set<TowerDefense::PathCell*>		   PathCellsSet;
	
private:

	ShotsSet			mShots;
	EnemiesSet			mEnemies;
	TowersSet			mTowers;
	TowerSlotsSet		mTowerSlots;
	BackGroundCellsSet	mBackGroundCells;
	PathCellsSet		mPathCells;

	// ���������� ������� �� �����
	void addClickableUnit(FieldUnit * unit);

	// �������������� ��������
	// � ��� ����� � �� ������
	void delUnit(FieldUnit * unit);

public:

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

	// ������ �� �����
	const BattleMap& getClickableUnits();
	

	virtual ~BattleField();

private:
	const UnitsOnLayer& getUnitsOnLayer(float layer) {
		return mBattleField[layer];
	}
	const BattleMap& getBattleMap();

	typedef std::pair<TowerDefense::Shot*, TowerDefense::Enemy*> ConnectedPair;
	typedef std::list<ConnectedPair>							 ConnectionsList;

	ConnectionsList mUnitConnections;
	BattleMap       mBattleField;
	BattleMap       mClickableUnits;
};

#endif