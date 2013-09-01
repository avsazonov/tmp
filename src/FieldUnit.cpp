#include "FieldUnit.h"
#include "LayerConstants.h"

#include <sstream>
#include <cstdlib>

using namespace TowerDefense;

Tower::Tower(float attackSpeed, float damage, float radius, int type) : FieldUnit("Tower") {

	// установка начальных настроек башни

	setSetting("attack_speed", attackSpeed);
	setSetting("damage", damage);
	setSetting("radius", radius);

	setSetting("size_x", 1.f);
	setSetting("size_y", 1.f);
	setSetting("layer",  gTowerLayer);

	setSetting("last_shot", 0.f);

	setSetting("type", float(type));
	// определяем название ресурса для отрисовки
	std::stringstream res_name;
	res_name << "tower_" << int(getSetting("type").getValue()) << std::flush;
	mResourceName = res_name.str();
}

void Tower::doRender() const {
	// отрисовываем башню в клетке
	mDrawHelper.drawSprite(mResourceName, getSetting("x").getValue(), getSetting("y").getValue());
}

bool Tower::isMyAreaClicked(float x, float y) const {
	// стандартная проверка на попадание в область
	return (x - getSetting("x").getValue() < getSetting("size_x").getValue()) &&
		   (y - getSetting("y").getValue() < getSetting("size_y").getValue()) &&
		   (x - getSetting("x").getValue() > 0) &&
		   (y - getSetting("y").getValue() > 0);
}

void TowerSlot::doRender() const {
	// отрисовываем слот
	mDrawHelper.drawSprite("tower_slot", getSetting("x").getValue(), getSetting("y").getValue());
}

TowerSlot::TowerSlot(float x, float y) : FieldUnit("TowerSlot") {

	// настройки слота

	setSetting("x", x);
	setSetting("y", y);
	setSetting("size_x", 1.f);
	setSetting("size_y", 1.f);

	// тот же слой, что и у башен
	setSetting("layer", gTowerLayer);
}

bool TowerSlot::isMyAreaClicked(float x, float y) const {
	// стандартная проверка на попадание в область
	return (x - getSetting("x").getValue() < getSetting("size_x").getValue()) &&
		   (y - getSetting("y").getValue() < getSetting("size_y").getValue()) &&
		   (x - getSetting("x").getValue() > 0) &&
		   (y - getSetting("y").getValue() > 0);
}

Enemy::Enemy(float HP, float speed, int type) : FieldUnit("Enemy") {

	// установка настроек "врагов"

	setSetting("HP", HP);

	// настройка текущего уровня жизни
	setSetting("current_HP", HP);

	setSetting("speed", speed);

	// размер вражеского юнита
	// в соотвествии со спрайтом
	setSetting("size_x", 3.f/5.f);
	setSetting("size_y", 3.f/5.f);

	// сдвиг спрайта, нужен для его центрирования в клетке
	setSetting("draw_delta_x", (1.f - getSetting("size_x").getValue())/2);
	setSetting("draw_delta_y", (1.f - getSetting("size_y").getValue())/2);

	setSetting("layer", gEnemyLayer);

	setSetting("type", float(type));

	// определяем имя ресурса
	// в соответствии с типом
	std::stringstream res_name;
	res_name << "enemy_" << int(getSetting("type").getValue()) << std::flush;
	mResourceName = res_name.str();
}

void Enemy::doRender() const {

	// рисуем самого врага
	mDrawHelper.drawSprite(mResourceName, 
		getSetting("x").getValue() + getSetting("draw_delta_x").getValue(), 
		getSetting("y").getValue() + getSetting("draw_delta_y").getValue(), 
		getSetting("size_x").getValue(), 
		getSetting("size_y").getValue());

	// два прямоугольника отражают уровень жизни
	mDrawHelper.drawPrimitive(DrawHelper::PRIMITIVE_RECTANGLE, 
		getSetting("x").getValue() + getSetting("draw_delta_x").getValue(), 
		getSetting("y").getValue() + getSetting("draw_delta_x").getValue(),	
		getSetting("size_x").getValue(), 
		0.1f,								// маленький размер по y
		DrawHelper::COLOR_BLUE);

	float current_hp = getSetting("size_x").getValue() * getSetting("current_HP").getValue() / getSetting("HP").getValue();
	if (current_hp > 0) 
		mDrawHelper.drawPrimitive(DrawHelper::PRIMITIVE_RECTANGLE, 
		getSetting("x").getValue() + getSetting("draw_delta_x").getValue(), 
		getSetting("y").getValue() + getSetting("draw_delta_y").getValue(), 
		current_hp,							// единственная разница - по х тем меньше, чем меньше жизни
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

	// установка настройки клетки фона

	setSetting("x", x);
	setSetting("y", y);
	setSetting("size_x", size_x);
	setSetting("size_y", size_y);

	setSetting("layer", gBackGroundLayer);

	// случайным образом определяем фон
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
	// также определяем ресурс по типу
	std::stringstream res_name;
	res_name << "bullet_" << int(getSetting("type").getValue()) << std::flush;

	// рисуем
	// фон (255, 0, 255) - магический в SDK, он является прозрачным
	// поэтому выстрелы у нас прозрачные (да и фон врагов тоже)
	mDrawHelper.drawSprite(res_name.str(), 
		getSetting("x").getValue(), 
		getSetting("y").getValue(), 
		getSetting("size_x").getValue(), 
		getSetting("size_y").getValue());
}

void BattleField::addUnit(FieldUnit * unit) {
	if (!unit)
		return;

	// добавление объекта на карту
	// сразу сортируем по слоям
	float layer = unit->getSetting("layer").getValue();
	mBattleField[layer].insert(unit);

	// ну и если он нажимабельный, отдельно запоминаем
	if (unit->isClickable())
		mClickableUnits[layer].insert(unit);
}

void BattleField::delUnit(FieldUnit * unit) {
	if (!unit)
		return;

	BattleField::BattleMap::iterator layer_iterator, clickable_set_layer_iterator;

	// Ищем подходящий уровень для объект
	layer_iterator = mBattleField.find(unit->getSetting("layer").getValue());
	clickable_set_layer_iterator = mClickableUnits.find(unit->getSetting("layer").getValue());

	// Не нашли - просто удаляем объект
	if (mBattleField.end() == layer_iterator) {
		// В нажимабельных не смотрим, так как туда добавляет addUnit
		delete unit;
		return;
	}

	// Удаляем сам элемент во всех множествах
	BattleField::UnitsOnLayer::iterator unit_index;

	// если слой нашелся в нажимабельных
	if (unit->isClickable() && (mClickableUnits.end() != clickable_set_layer_iterator)) 
		// если сам объект нашелся в слое
		if ((unit_index = clickable_set_layer_iterator->second.find(unit)) != clickable_set_layer_iterator->second.end())
			// удаляем запись об объекте
			clickable_set_layer_iterator->second.erase(unit_index);

	// объект есть на карте, так как иначе мы бы уже вышли
	// ищем его индекс в слое
	if ((unit_index = layer_iterator->second.find(unit)) != layer_iterator->second.end()) 
		// и удаляем
		layer_iterator->second.erase(unit_index);

	// Также удаляем все упоминания
	delConnection(unit);

	// Удаляем в любом случае, так как попросили удалить
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
		// Удаляем все объекты в массиве из памяти
		// Не трогаем clickable units, так как они
		// содержат те же указатели
		for (unit = layer->second.begin();
			unit != layer->second.end();
			++unit)
			delete (*unit);
}

void BattleField::addConnection(FieldUnit * firstUnit, FieldUnit * secondUnit) {
	// Добавляем только ненулевые объекты
	if (firstUnit && secondUnit)
		mUnitConnections.push_back(BattleField::ConnectedPair(firstUnit, secondUnit));
}

void BattleField::delConnection(FieldUnit * unit) {
	std::list<BattleField::ConnectionsList::iterator> entries_to_delete;

	// Сначала набираем список на удаление
	for (BattleField::ConnectionsList::iterator entry = mUnitConnections.begin();
		entry != mUnitConnections.end();
		++entry)
		// удаляем любую связь!
		// что слева, что справа
		if ((entry->first == unit) || (entry->second == unit)) 
			entries_to_delete.push_back(entry);

	// удаляем по списку
	for (std::list<BattleField::ConnectionsList::iterator>::iterator pair_to_delete = entries_to_delete.begin();
		pair_to_delete != entries_to_delete.end();
		++pair_to_delete)
		mUnitConnections.erase(*pair_to_delete);
}

FieldUnit * BattleField::getConnectedUnit(FieldUnit * unit) {
	// поиск связи по первому вхождению
	for (BattleField::ConnectionsList::iterator entry = mUnitConnections.begin();
		entry != mUnitConnections.end();
		++entry)
		// считается найденным, если он ссылается (а не на него)
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

