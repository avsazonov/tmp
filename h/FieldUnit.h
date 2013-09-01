#ifndef FIELDUNIT_H_INCLUDED
#define FIELDUNIT_H_INCLUDED

#include "GameObject.h"
#include <set>
#include <list>

// Абстрактный класс для всех отображаемых на поле объектов.
// Требует спецификации имени объекта.
// Каждый объект на поле имеет координату z ("layer").
// Она определяет порядок отрисовки (от меньшего к большему).
// По умолчанию 0, и уровень 0 - не отрисовывается.
class FieldUnit : public GameObject, public Renderable {

	std::string mObjectName;

public:
	// без имени - никакой объект не создать
	FieldUnit(const std::string& objectName) {
		mObjectName = objectName;

		// всегда ставим по умолчанию layer = 0
		setSetting("layer", Setting(0));
	}

	std::string getObjectName() const {
		return mObjectName;
	}

	// переопределяем метод отрисовки
	void render() const {
		if (getSetting("layer").isSet() && (getSetting("layer").getValue() > 0.f))
			doRender();
	}

protected:

	// отвечает за отображение
	virtual void doRender() const = 0;
};

namespace TowerDefense {

	// Объекты башни. Характеризуются скорострельностью, уроном и дальностью поражения.
	// тип определяет ресурс для отображения
	class Tower : public FieldUnit {
		std::string mResourceName;
	public:
		Tower(float attackSpeed, float damage, float radius, int type);
		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	protected:
		virtual void doRender() const;
	};

	// Объект пустого места для башни
	class TowerSlot : public FieldUnit {
	public:
		TowerSlot(float x, float y);
		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	protected:
		virtual void doRender() const;
	};

	// Объекты врагов. Характеризуются начальным жизненным уровнем и скоростью движения.
	// тип определяет ресурс для отображения
	class Enemy : public FieldUnit {
		std::string mResourceName;
	public:
		Enemy(float HP, float speed, int type);

	protected:
		virtual void doRender() const;
	};

	// Объект одной клетки дорожки, по которой может двигаться враг.
	// Задаются координаты клетки.
	class PathCell : public FieldUnit {
	public:
		PathCell(float x, float y);

	protected:
		virtual void doRender() const;
	};

	// Объект одной клетки фона (задается координатами и размером).
	class BackGroundCell : public FieldUnit {
		std::string mResourceName;
	public:
		BackGroundCell(float x, float y, float size_x, float size_y);

	protected:
		virtual void doRender() const;
	};

	// Объект выстрела.
	class Shot : public FieldUnit {
	public:
		Shot();

	protected:
		virtual void doRender() const;
	};
}

// Игровое поле состоит из привязанных по уровням отображения
// списков объектов на нём.
// Сам удаляет все объекты!
// То есть если явно вызван DelUnit, или ~BattleField - для всех
// объектов вызывается delete. Таким образом, нельзя создавать 
// объекты на стеке, если их необходимо добавлять на игровое поле.
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

	// добавление объекта на карту
	void addUnit(FieldUnit * unit);

public:

	// соответственно удаление
	// в том числе и из памяти
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

	// добавление связи между двумя объектами
	void addConnection(FieldUnit * firstUnit, FieldUnit * secondUnit);

	// удаление связей объекта
	// сам объект не трогается
	// удаляются любые связи!
	void delConnection(FieldUnit * unit);

	// получение связанного объекта
	// связь найдена, если найден первый объект,
	// то есть тот, который ссылается, а не на
	// который ссылаются. firstUnit из addConnection
	FieldUnit * getConnectedUnit(FieldUnit * unit);

	// ссылки на карты
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