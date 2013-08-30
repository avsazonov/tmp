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
class BattleField : public GameObject {

public:
	typedef std::set<FieldUnit*>		   UnitsOnLayer;
	typedef std::map<float, UnitsOnLayer>  BattleMap;

	std::string getObjectName() const { return "BattleField"; }

	// ���������� ������� �� �����
	void addUnit(FieldUnit * unit);

	// �������������� ��������
	// � ��� ����� � �� ������
	void delUnit(FieldUnit * unit);

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
	const UnitsOnLayer& getUnitsOnLayer(float layer) {
		return mBattleField[layer];
	}

	virtual ~BattleField();

private:
	typedef std::pair<FieldUnit*, FieldUnit*> ConnectedPair;
	typedef std::list<ConnectedPair>          ConnectionsList;

	ConnectionsList mUnitConnections;
	BattleMap       mBattleField;
	BattleMap       mClickableUnits;
};

#endif