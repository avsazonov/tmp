#include "FieldUnit.h"
#include "LayerConstants.h"

#include <sstream>
#include <cstdlib>

#include "WorldCreator.h"

using namespace TowerDefense;

Tower::Tower(float attackSpeed, float damage, float radius, int type, const WorldCreator * const creator) : 
	FieldUnit("Tower"), mWorldCreator(creator) {

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

// TODO: move to utilities.h
inline float square(float x) { return x*x; }

TowerDefense::Shot * Tower::shootEnemy(const std::set<TowerDefense::Enemy*> &enemies, float currentTime, int timeDelta) {
		// ��������� ������� � ���������� �������� ������ �� ������, ��� ����� ���������
		if ((currentTime - getSetting("last_shot").getValue()) < (getSetting("attack_speed").getValue() * 1000))
			// ��������
			return 0;

		TowerDefense::Shot * shot = 0;

		// �� ��������, ������ ���� ����� �� ���� ��������, �� ���� � ������� ����������� (� ����������)
		for (BattleField::EnemiesSet::iterator unit_iterator = enemies.begin();
			unit_iterator != enemies.end();
			++unit_iterator) {

				TowerDefense::Enemy * unit = *unit_iterator;

				// ���������� ������ ������
				if (unit->getSetting("current_HP").getValue() <= 0)
					continue;
				
				// ��������� ����� - � ����� ����, ����� ��������� ���-����� �� ��������
				// � ������ ����� �����
				float tower_x = getSetting("x").getValue() + 0.5f;
				float tower_y = getSetting("y").getValue() + 0.3f;
				float unit_x  = unit->getSetting("x").getValue();
				float unit_y  = unit->getSetting("y").getValue();

				// ��������� ���������� �� �����
				if ((square(tower_x - unit_x) + square(unit_y - tower_y)) < square(getSetting("radius").getValue())) {
					// ���� ������ ��������� ��� ����� - �� ������� �������
					shot = mWorldCreator->createShot();
					if (!shot)
						continue;
					
					shot->setSetting("damage",  getSetting("damage").getValue());
					shot->setSetting("type",    getSetting("type").getValue());

					// ������������� ��������� x � y
					shot->setSetting("tower_x", tower_x);
					shot->setSetting("tower_y", tower_y);
					
					shot->setSetting("x", tower_x);
					shot->setSetting("y", tower_y);
					
					// �������������, ��� �������� ����
					// �� ���� �������� � �����������
					std::pair<float, float> target; bool do_update = false;
					// ����� ���� �� �������� ��� ���:
					// target = predictNextEnemyStep(unit, gShotLife, do_update);
					// �� ��� ����� �������� ������ ��� ��������� ��������� ������������
					// ����� ���������� ������ � ������ �������. ������� ������� �������
					
					// �������� ���������
					float current_way_point = unit->getSetting("way_point_number").getValue();

					// �������� �������������, ��� �������� ���� �� ����� ������ �������
					for (int i = 0; i < gShotLife/timeDelta; ++i) {
						target = unit->predictNextStep(timeDelta, do_update);
						unit->setSetting("x", target.first);
						unit->setSetting("y", target.second);
						if (do_update)
							unit->setSetting("way_point_number", unit->getSetting("way_point_number").getValue() + 1);
					}

					// ������ ��������� �� �����
					unit->setSetting("way_point_number", current_way_point);
					unit->setSetting("x", unit_x); unit->setSetting("y", unit_y);

					// ����� ���������
					shot->setSetting("target_x", target.first + unit->getSetting("size_x").getValue()/2);
					shot->setSetting("target_y", target.second + unit->getSetting("size_y").getValue()/2);

					shot->addLink(unit);
					setSetting("last_shot", currentTime);

					// ���������� - ������� �� ����� �� ������
					break;
				}
		}

		return shot;
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

Enemy::Enemy(float HP, float speed, int type, const WorldCreator * const creator) : FieldUnit("Enemy"), mWorldCreator(creator) {

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

// TODO: move to utilities.h
// �������� ��� x, y ����� x1, y1 � x2, y2 (� ��������������)
inline float max(float a, float b) { return a > b ? a : b; }
inline float min(float a, float b) { return a < b ? a : b; }
inline bool checkInBounds(float x1, float y1, float x, float y, float x2, float y2) {
	return (min(x, max(x1, x2)) == x) && (min(y, max(y1, y2)) == y) && (max(x, min(x1, x2)) == x) && (max(y, min(y1, y2)) == y);
}

std::pair<float, float> Enemy::predictNextStep(int timeDelta, bool &updateWaypoint) {
	updateWaypoint = false;

	// �������� ��������� ����� ���������� �� �����
	WorldCreator::WayPoint entry_point, next_point, current_point;
	entry_point.first = getSetting("entry_x").getValue();
	entry_point.second = getSetting("entry_y").getValue();
	int way_point_number = int(getSetting("way_point_number").getValue());
	
	current_point = mWorldCreator->getWayPoint(entry_point, way_point_number);
	next_point = mWorldCreator->getWayPoint(entry_point, way_point_number + 1);

	std::pair<float, float> point;
	point.first = getSetting("x").getValue();
	point.second = getSetting("y").getValue();

	// ���� ��������� - ��� ����� - �� ���������� �������
	if ((next_point.first == entry_point.first) && (next_point.second == entry_point.second))
		return point;

	// ��������� ���
	float delta_x = (next_point.first - current_point.first) * 
		getSetting("speed").getValue() / (1000 / timeDelta);  
	float delta_y = (next_point.second - current_point.second) *
		getSetting("speed").getValue() / (1000 / timeDelta);  

	// ���� ������� �� ������� �������� ��������������, ���� ��� ��������
	if (!checkInBounds(next_point.first, next_point.second, 
		            point.first + delta_x, point.second + delta_y, 
					current_point.first, current_point.second)) {
		updateWaypoint = true;

		// ��� ���������� ���������
		// ������ (x + delta_x, y + delta_y) ����� �� ������� �������� ��������������, ������������� ������������ �������
		// ���� ����, ��� ���� �������� ����������� �����, ��� ���� � ����������������� ������
		// ����� ������� (delta_x, delta_y) �������������� ������ �� ������� ������������� �������������
		// ���� �� �����, ������ ������������� ���� ������ - ����� ��������� ����������� �����
		// �������� � ����������� ������� - ����, ���� ��������� ��� ����� ��������� ����������� �����

		float move_distance = sqrtf(square(delta_x) + square(delta_y));
		float move_distance_new = sqrtf(square(next_point.first - point.first - delta_x) + square(next_point.second - point.second - delta_y));
		point = next_point; current_point = next_point;
		next_point = mWorldCreator->getWayPoint(entry_point, way_point_number + 2);
		delta_x =  (next_point.first - current_point.first) * 
					getSetting("speed").getValue() / (1000 / timeDelta) *  
					move_distance_new / move_distance;
		delta_y =  (next_point.second - current_point.second) *
					getSetting("speed").getValue() / (1000 / timeDelta) *  
					move_distance_new / move_distance;

		// ���� ����� �� ���������� ��������� ��������, �������� ���� ������ ������
	}

	point.first += delta_x;
	point.second += delta_y;

	return point;
}

void Enemy::move(int timeDelta) {
	bool update_waypoint = false;
	std::pair<float, float> next_enemy_step = predictNextStep(timeDelta, update_waypoint);
	setSetting("x", next_enemy_step.first);
	setSetting("y", next_enemy_step.second);

	// ��������� �������� �������� - �� ������ ��������� ����������� �����
	// ��� predict �� ��������� ��-������ ������ ��� �� predict, � ��-������
	// �� ������������ ��� ��� ���������� ���� ��������
	if (update_waypoint)
		setSetting("way_point_number", getSetting("way_point_number").getValue() + 1);
}

bool Enemy::runOut() {
	return (getSetting("x").getValue() <= -getSetting("size_x").getValue()) ||
		   (getSetting("y").getValue() <= -getSetting("size_y").getValue()) ||
		   (getSetting("x").getValue() >= mWorldCreator->getCellsX() + getSetting("size_x").getValue()) ||
		   (getSetting("y").getValue() >= mWorldCreator->getCellsY() + getSetting("size_y").getValue()); 
}

bool Enemy::isDestroyed() {
	return getSetting("current_HP").getValue() <= 0;
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

Shot::Shot() : FieldUnit("Shot"), mLinkedUnit(0) {
	setSetting("size_x", 0.25f);
	setSetting("size_y", 0.25f);
	setSetting("layer", gShotLayer);
}

void Shot::move(int timeDelta) {

	if (reachedTarget())
		return;

	float delta_x = (getSetting("target_x").getValue() - getSetting("tower_x").getValue()) / (gShotLife / timeDelta); 
	float delta_y = (getSetting("target_y").getValue() - getSetting("tower_y").getValue()) / (gShotLife / timeDelta);

	setSetting("x", getSetting("x").getValue() + delta_x);
	setSetting("y", getSetting("y").getValue() + delta_y);
}

void Shot::damageTarget() {
	if (getLinked()) 
		getLinked()->setSetting("current_HP", getLinked()->getSetting("current_HP").getValue() - getSetting("damage").getValue());
}

bool Shot::reachedTarget() {

	return !checkInBounds(getSetting("target_x").getValue(), getSetting("target_y").getValue(),
						  getSetting("x").getValue(), getSetting("y").getValue(),
						  getSetting("tower_x").getValue(), getSetting("tower_y").getValue());
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

void BattleField::addClickableUnit(FieldUnit * unit) {
	if (!unit)
		return;

	// ���������� ������� �� �����
	// ����� ��������� �� �����
	float layer = unit->getSetting("layer").getValue();
	//mBattleField[layer].insert(unit);

	// �� � ���� �� �������������, �������� ����������
	if (unit->isClickable())
		mClickableUnits[layer].insert(unit);
}

void BattleField::delUnit(FieldUnit * unit) {
	if (!unit)
		return;

	BattleField::BattleMap::iterator layer_iterator, clickable_set_layer_iterator;

	clickable_set_layer_iterator = mClickableUnits.find(unit->getSetting("layer").getValue());

	// ������� ��� ������� �� ���� ����������
	BattleField::UnitsOnLayer::iterator unit_index;

	// ���� ���� ������� � �������������
	if (unit->isClickable() && (mClickableUnits.end() != clickable_set_layer_iterator)) 
		// ���� ��� ������ ������� � ����
		if ((unit_index = clickable_set_layer_iterator->second.find(unit)) != clickable_set_layer_iterator->second.end())
			// ������� ������ �� �������
			clickable_set_layer_iterator->second.erase(unit_index);

	// ������� � ����� ������, ��� ��� ��������� �������
	delete unit;
}

const BattleField::BattleMap& BattleField::getClickableUnits() {
	return mClickableUnits;
}

BattleField::~BattleField() {
	BattleField::BattleMap::iterator    layer;
	BattleField::UnitsOnLayer::iterator unit;

	for (ShotsSet::iterator shot_iterator = mShots.begin(); shot_iterator != mShots.end(); ++shot_iterator)
		delete *shot_iterator;
	
	for (PathCellsSet::iterator pathcell_iterator = mPathCells.begin(); pathcell_iterator != mPathCells.end(); ++pathcell_iterator)
		delete *pathcell_iterator;

	for (BackGroundCellsSet::iterator bgcell_iterator = mBackGroundCells.begin(); bgcell_iterator != mBackGroundCells.end(); ++bgcell_iterator)
		delete *bgcell_iterator;
	
	for (EnemiesSet::iterator enemy_iterator = mEnemies.begin(); enemy_iterator != mEnemies.end(); ++enemy_iterator)
		delete *enemy_iterator;

	for (TowersSet::iterator tower_iterator = mTowers.begin(); tower_iterator != mTowers.end(); ++tower_iterator)
		delete *tower_iterator;
	
	for (TowerSlotsSet::iterator slot_iterator = mTowerSlots.begin(); slot_iterator != mTowerSlots.end(); ++slot_iterator)
		delete *slot_iterator;
}


void BattleField::addShot(TowerDefense::Shot* shot) {
	if (0 != shot)
		mShots.insert(shot);
	addClickableUnit(shot);
}

void BattleField::delShot(TowerDefense::Shot* shot) {
	mShots.erase(shot);
	delUnit(shot);
}

void BattleField::addEnemy(TowerDefense::Enemy* enemy) {
	if (0 != enemy)
		mEnemies.insert(enemy);
	addClickableUnit(enemy);
}

void BattleField::delEnemy(TowerDefense::Enemy* enemy) {
	mEnemies.erase(enemy);
	delUnit(enemy);
}

void BattleField::addTower(TowerDefense::Tower* tower) {
	if (0 != tower)
		mTowers.insert(tower);
	addClickableUnit(tower);
}

void BattleField::delTower(TowerDefense::Tower* tower) {
	mTowers.erase(tower);
	delUnit(tower);
}

void BattleField::addTowerSlot(TowerDefense::TowerSlot* towerSlot) {
	if (0 != towerSlot)
		mTowerSlots.insert(towerSlot);
	addClickableUnit(towerSlot);
}

void BattleField::delTowerSlot(TowerDefense::TowerSlot* towerSlot) {
	mTowerSlots.erase(towerSlot);
	delUnit(towerSlot);
}

void BattleField::addBackGroundCell(TowerDefense::BackGroundCell* bgCell) {
	if (0 != bgCell)
		mBackGroundCells.insert(bgCell);
	addClickableUnit(bgCell);
}

void BattleField::delBackGroundCell(TowerDefense::BackGroundCell* bgCell) {
	mBackGroundCells.erase(bgCell);
	delUnit(bgCell);
}

void BattleField::addPathCell(TowerDefense::PathCell* pathCell) {
	if (0 != pathCell)
		mPathCells.insert(pathCell);
	addClickableUnit(pathCell);
}

void BattleField::delPathCell(TowerDefense::PathCell* pathCell) {
	mPathCells.erase(pathCell);
	delUnit(pathCell);
}

const BattleField::ShotsSet& BattleField::getShots() {
	return mShots;
}

const BattleField::EnemiesSet& BattleField::getEnemies() {
	return mEnemies;
}

const BattleField::TowersSet& BattleField::getTowers() {
	return mTowers;
}

const BattleField::TowerSlotsSet& BattleField::getTowerSlots() {
	return mTowerSlots;
}

const BattleField::BackGroundCellsSet& BattleField::getBackGroundCells() {
	return mBackGroundCells;
}

const BattleField::PathCellsSet& BattleField::getPathCells() {
	return mPathCells;
}
