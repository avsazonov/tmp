#include "FieldUnit.h"
#include "LayerConstants.h"

#include <sstream>
#include <cstdlib>

using namespace TowerDefense;

Tower::Tower(float attackSpeed, float damage, float radius, int type) : FieldUnit("Tower") {

	// ��������� ��������� �������� �����

	setSetting("attack_speed", attackSpeed);
	setSetting("damage", damage);
	setSetting("radius", radius);

	setSetting("size_x", 1.f);
	setSetting("size_y", 1.f);
	setSetting("layer",  gTowerLayer);

	setSetting("last_shot", 0.f);

	setSetting("type", float(type));
	// ���������� �������� ������� ��� ���������
	std::stringstream res_name;
	res_name << "tower_" << int(getSetting("type").getValue()) << std::flush;
	mResourceName = res_name.str();
}

void Tower::doRender() const {
	// ������������ ����� � ������
	mDrawHelper.drawSprite(mResourceName, getSetting("x").getValue(), getSetting("y").getValue());
}

bool Tower::isMyAreaClicked(float x, float y) const {
	// ����������� �������� �� ��������� � �������
	return (x - getSetting("x").getValue() < getSetting("size_x").getValue()) &&
		   (y - getSetting("y").getValue() < getSetting("size_y").getValue()) &&
		   (x - getSetting("x").getValue() > 0) &&
		   (y - getSetting("y").getValue() > 0);
}

void TowerSlot::doRender() const {
	// ������������ ����
	mDrawHelper.drawSprite("tower_slot", getSetting("x").getValue(), getSetting("y").getValue());
}

TowerSlot::TowerSlot(float x, float y) : FieldUnit("TowerSlot") {

	// ��������� �����

	setSetting("x", x);
	setSetting("y", y);
	setSetting("size_x", 1.f);
	setSetting("size_y", 1.f);

	// ��� �� ����, ��� � � �����
	setSetting("layer", gTowerLayer);
}

bool TowerSlot::isMyAreaClicked(float x, float y) const {
	// ����������� �������� �� ��������� � �������
	return (x - getSetting("x").getValue() < getSetting("size_x").getValue()) &&
		   (y - getSetting("y").getValue() < getSetting("size_y").getValue()) &&
		   (x - getSetting("x").getValue() > 0) &&
		   (y - getSetting("y").getValue() > 0);
}

Enemy::Enemy(float HP, float speed, int type) : FieldUnit("Enemy") {

	// ��������� �������� "������"

	setSetting("HP", HP);

	// ��������� �������� ������ �����
	setSetting("current_HP", HP);

	setSetting("speed", speed);

	// ������ ���������� �����
	// � ����������� �� ��������
	setSetting("size_x", 3.f/5.f);
	setSetting("size_y", 3.f/5.f);

	// ����� �������, ����� ��� ��� ������������� � ������
	setSetting("draw_delta_x", (1.f - getSetting("size_x").getValue())/2);
	setSetting("draw_delta_y", (1.f - getSetting("size_y").getValue())/2);

	setSetting("layer", gEnemyLayer);

	setSetting("type", float(type));

	// ���������� ��� �������
	// � ������������ � �����
	std::stringstream res_name;
	res_name << "enemy_" << int(getSetting("type").getValue()) << std::flush;
	mResourceName = res_name.str();
}

void Enemy::doRender() const {

	// ������ ������ �����
	mDrawHelper.drawSprite(mResourceName, 
		getSetting("x").getValue() + getSetting("draw_delta_x").getValue(), 
		getSetting("y").getValue() + getSetting("draw_delta_y").getValue(), 
		getSetting("size_x").getValue(), 
		getSetting("size_y").getValue());

	// ��� �������������� �������� ������� �����
	mDrawHelper.drawPrimitive(DrawHelper::PRIMITIVE_RECTANGLE, 
		getSetting("x").getValue() + getSetting("draw_delta_x").getValue(), 
		getSetting("y").getValue() + getSetting("draw_delta_x").getValue(),	
		getSetting("size_x").getValue(), 
		0.1f,								// ��������� ������ �� y
		DrawHelper::COLOR_BLUE);

	float current_hp = getSetting("size_x").getValue() * getSetting("current_HP").getValue() / getSetting("HP").getValue();
	if (current_hp > 0) 
		mDrawHelper.drawPrimitive(DrawHelper::PRIMITIVE_RECTANGLE, 
		getSetting("x").getValue() + getSetting("draw_delta_x").getValue(), 
		getSetting("y").getValue() + getSetting("draw_delta_y").getValue(), 
		current_hp,							// ������������ ������� - �� � ��� ������, ��� ������ �����
		0.1f, 
		DrawHelper::COLOR_RED);
}

PathCell::PathCell(float x, float y) : FieldUnit("PathCell") {
	setSetting("x", Setting(x));
	setSetting("y", Setting(y));

	setSetting("layer", Setting(gPathCellLayer));
}

void PathCell::doRender() const {
	mDrawHelper.drawSprite("pathcell", getSetting("x").getValue(), getSetting("y").getValue());
}

BackGroundCell::BackGroundCell(float x, float y, float size_x, float size_y) : FieldUnit("BackGroundCell") {

	// ��������� ��������� ������ ����

	setSetting("x", x);
	setSetting("y", y);
	setSetting("size_x", size_x);
	setSetting("size_y", size_y);

	setSetting("layer", gBackGroundLayer);

	// ��������� ������� ���������� ���
	std::stringstream res_name;
	res_name << "sprite_bg";
	res_name << (rand() % 2 + 1);
	res_name << std::flush;
	mResourceName = res_name.str();
}

void BackGroundCell::doRender() const {
	mDrawHelper.drawSprite(mResourceName, 
		getSetting("x").getValue(), getSetting("y").getValue(),
		getSetting("size_x").getValue(), getSetting("size_y").getValue(),
		true // blend with the background
	);
}

Shot::Shot() : FieldUnit("Shot") {
	setSetting("size_x", 0.25f);
	setSetting("size_y", 0.25f);
	setSetting("layer", gShotLayer);
}

void Shot::doRender() const {
	// ����� ���������� ������ �� ����
	std::stringstream res_name;
	res_name << "bullet_" << int(getSetting("type").getValue()) << std::flush;

	// ������
	// ��� (255, 0, 255) - ���������� � SDK, �� �������� ����������
	// ������� �������� � ��� ���������� (�� � ��� ������ ����)
	mDrawHelper.drawSprite(res_name.str(), 
		getSetting("x").getValue(), 
		getSetting("y").getValue(), 
		getSetting("size_x").getValue(), 
		getSetting("size_y").getValue());
}

void BattleField::addUnit(FieldUnit * unit) {
	if (!unit)
		return;

	// ���������� ������� �� �����
	// ����� ��������� �� �����
	float layer = unit->getSetting("layer").getValue();
	mBattleField[layer].insert(unit);

	// �� � ���� �� �������������, �������� ����������
	if (unit->isClickable())
		mClickableUnits[layer].insert(unit);
}

void BattleField::delUnit(FieldUnit * unit) {
	if (!unit)
		return;

	BattleField::BattleMap::iterator layer_iterator, clickable_set_layer_iterator;

	// ���� ���������� ������� ��� ������
	layer_iterator = mBattleField.find(unit->getSetting("layer").getValue());
	clickable_set_layer_iterator = mClickableUnits.find(unit->getSetting("layer").getValue());

	// �� ����� - ������ ������� ������
	if (mBattleField.end() == layer_iterator) {
		// � ������������� �� �������, ��� ��� ���� ��������� addUnit
		delete unit;
		return;
	}

	// ������� ��� ������� �� ���� ����������
	BattleField::UnitsOnLayer::iterator unit_index;

	// ���� ���� ������� � �������������
	if (unit->isClickable() && (mClickableUnits.end() != clickable_set_layer_iterator)) 
		// ���� ��� ������ ������� � ����
		if ((unit_index = clickable_set_layer_iterator->second.find(unit)) != clickable_set_layer_iterator->second.end())
			// ������� ������ �� �������
			clickable_set_layer_iterator->second.erase(unit_index);

	// ������ ���� �� �����, ��� ��� ����� �� �� ��� �����
	// ���� ��� ������ � ����
	if ((unit_index = layer_iterator->second.find(unit)) != layer_iterator->second.end()) 
		// � �������
		layer_iterator->second.erase(unit_index);

	// ����� ������� ��� ����������
	delConnection(unit);

	// ������� � ����� ������, ��� ��� ��������� �������
	delete unit;
}

const BattleField::BattleMap& BattleField::getClickableUnits() {
	return mClickableUnits;
}

const BattleField::BattleMap& BattleField::getBattleMap() {
	return mBattleField;
}

BattleField::~BattleField() {
	BattleField::BattleMap::iterator    layer;
	BattleField::UnitsOnLayer::iterator unit;

	for (layer = mBattleField.begin();
		layer != mBattleField.end();
		++layer) 
		// ������� ��� ������� � ������� �� ������
		// �� ������� clickable units, ��� ��� ���
		// �������� �� �� ���������
		for (unit = layer->second.begin();
			unit != layer->second.end();
			++unit)
			delete (*unit);
}

void BattleField::addConnection(FieldUnit * firstUnit, FieldUnit * secondUnit) {
	// ��������� ������ ��������� �������
	if (firstUnit && secondUnit)
		mUnitConnections.push_back(BattleField::ConnectedPair(firstUnit, secondUnit));
}

void BattleField::delConnection(FieldUnit * unit) {
	std::list<BattleField::ConnectionsList::iterator> entries_to_delete;

	// ������� �������� ������ �� ��������
	for (BattleField::ConnectionsList::iterator entry = mUnitConnections.begin();
		entry != mUnitConnections.end();
		++entry)
		// ������� ����� �����!
		// ��� �����, ��� ������
		if ((entry->first == unit) || (entry->second == unit)) 
			entries_to_delete.push_back(entry);

	// ������� �� ������
	for (std::list<BattleField::ConnectionsList::iterator>::iterator pair_to_delete = entries_to_delete.begin();
		pair_to_delete != entries_to_delete.end();
		++pair_to_delete)
		mUnitConnections.erase(*pair_to_delete);
}

FieldUnit * BattleField::getConnectedUnit(FieldUnit * unit) {
	// ����� ����� �� ������� ���������
	for (BattleField::ConnectionsList::iterator entry = mUnitConnections.begin();
		entry != mUnitConnections.end();
		++entry)
		// ��������� ���������, ���� �� ��������� (� �� �� ����)
		if (entry->first == unit)
			return entry->second;
	return 0;
}

void BattleField::addShot(TowerDefense::Shot* shot) {
	if (0 != shot)
		mShots.insert(shot);
}

void BattleField::delShot(TowerDefense::Shot* shot) {
	mShots.erase(shot);
	delete shot;
}

void BattleField::addEnemy(TowerDefense::Enemy* enemy) {
	if (0 != enemy)
		mEnemies.insert(enemy);
}

void BattleField::delEnemy(TowerDefense::Enemy* enemy) {
	mEnemies.erase(enemy);
	delete enemy;
}

void BattleField::addTower(TowerDefense::Tower* tower) {
	if (0 != tower)
		mTowers.insert(tower);
}

void BattleField::delTower(TowerDefense::Tower* tower) {
	mTowers.erase(tower);
	delete tower;
}

void BattleField::addTowerSlot(TowerDefense::TowerSlot* towerSlot) {
	if (0 != towerSlot)
		mTowerSlots.insert(towerSlot);
}

void BattleField::delTowerSlot(TowerDefense::TowerSlot* towerSlot) {
	mTowerSlots.erase(towerSlot);
	delete towerSlot;
}

void BattleField::addBackGroundCell(TowerDefense::BackGroundCell* bgCell) {
	if (0 != bgCell)
		mBackGroundCells.insert(bgCell);
}

void BattleField::delBackGroundCell(TowerDefense::BackGroundCell* bgCell) {
	mBackGroundCells.erase(bgCell);
	delete bgCell;
}

void BattleField::addPathCell(TowerDefense::PathCell* pathCell) {
	if (0 != pathCell)
		mPathCells.insert(pathCell);
}

void BattleField::delPathCell(TowerDefense::PathCell* pathCell) {
	mPathCells.erase(pathCell);
	delete pathCell;
}

