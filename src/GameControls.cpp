#include "GameControls.h"

using namespace TowerDefense;

const float gPlayButtonXFactor = 1.8f;

void PlayButton::render() const {
	// Рисуем кнопку справа вверху
	if (int(getSetting("pause").getValue())) 
		mDrawHelper.drawSprite("start_btn", DrawHelper::getCellCenterX() * gPlayButtonXFactor, 0.f);
	else
		mDrawHelper.drawSprite("pause_btn", DrawHelper::getCellCenterX() * gPlayButtonXFactor, 0.f);
}

bool PlayButton::isMyAreaClicked(float x, float y) const {
	// Проверяем, попали ли кликом в кнопку
	return (0.f < x - DrawHelper::getCellCenterX() * gPlayButtonXFactor) && 
		   // 1.f - это размер кнопки - одна клетка 
		   (x - DrawHelper::getCellCenterX() * gPlayButtonXFactor < 1.f) && 
		   (y < 1.f)  && 
		   (0.f < y);
}

TowerChoiceMenu::TowerChoiceMenu() {
	// Ставим размер по х в 7 клеток
	setSetting("size_x", 7.f); 
	// По y - в 3.5 клетки
	setSetting("size_y", 3.5f); 

	// центрируем по X
	setSetting("x", DrawHelper::getCellCenterX() - 3.5f);  
	// центрируем по Y
	setSetting("y", DrawHelper::getCellCenterY() - 1.75f); 
}

void TowerChoiceMenu::render() const {
	// Рисуем спрайт меню
	mDrawHelper.drawSprite("choice_menu", 
		getSetting("x").getValue(), 
		getSetting("y").getValue(), 
		getSetting("size_x").getValue(), 
		getSetting("size_y").getValue()
	);
}

bool TowerChoiceMenu::isMyAreaClicked(float x, float y) const {
	// Проверяем попадание в область меню
	return (0.f < x - getSetting("x").getValue()) && 
		   (x - getSetting("x").getValue() < getSetting("size_x").getValue())  && 
		   (y - getSetting("y").getValue() < getSetting("size_y").getValue())  && 
		   (0.f < y - getSetting("y").getValue());
}

#include <sstream>
void TimerBar::render() const {
	// Создаём текст m:ss
	std::stringstream text_stream;
	text_stream <<   int(getSetting("minutes").getValue()) << ":" 
	            << ((int(getSetting("seconds").getValue()) < 10) ? "0" : std::string())
	            <<   int(getSetting("seconds").getValue()) << std::flush;

	// Отрисовываем в левом верхнем углу размером в одну клетку
	mDrawHelper.drawText(text_stream.str(), 0.f, 0.f);
}

void StatisticsBar::render() const {
	std::stringstream text_stream;
	text_stream << "Run out: " << int(getSetting("alive").getValue()) << "/5" << std::flush;

	// Отрисовываем текст в левом нижнем углу. 
	// Для этого берем max y и вычитаем клетку
	mDrawHelper.drawText(text_stream.str(), 0.f, DrawHelper::getCellCenterY() * 2 - 1.f);
}

void GameFinalStatistics::render() const {
	
	DrawHelper::COLOR bg_color;
	// В зависимости от того, выиграл ли игрок, выбираем цвет баннера
	bool win = getSetting("win").isSet() && (getSetting("win").getValue() > 0);
	
	if (win)
		bg_color = DrawHelper::COLOR_GREEN;
	else
		bg_color = DrawHelper::COLOR_RED;

	// Рисуем фон баннера
	// 7x5 клеток, со второй сверху по вертикали
	mDrawHelper.drawPrimitive(DrawHelper::PRIMITIVE_RECTANGLE,
		                      7.f, 2.f, 7.f, 7.f, bg_color);

	// Рисуем текст о результате в самом начале
	if (win)
		mDrawHelper.drawText("You win!", 7.f, 2.f);
	else
		mDrawHelper.drawText("You lost", 7.f, 2.f);

	// Здесь можно сделать цикл при необходимости
	// Настройкой задавать количество типов
	// И запрашивать заданные настройки в цикле

	// Но с другой стороны, для отображения всегда
	// неплохо бы знать заранее, сколько строк потребуется

	std::stringstream row_1, row_2, row_3, row_4;
	// Сколько убили первого типа
	row_1 << "Killed type 1: " << int(getSetting("killed_type_1").getValue()) << std::flush;
	// Сколько второго
	row_2 << "Killed type 2: " << int(getSetting("killed_type_2").getValue()) << std::flush;
	// Сколько третьего
	row_3 << "Killed type 3: " << int(getSetting("killed_type_3").getValue()) << std::flush;
	// И сколько убежало
	row_4 << "Run out: " << int(getSetting("alive").getValue()) << std::flush;
	
	mDrawHelper.drawText(row_1.str(), 7.f, 3.f);
	mDrawHelper.drawText(row_2.str(), 7.f, 4.f);
	mDrawHelper.drawText(row_3.str(), 7.f, 5.f);
	mDrawHelper.drawText(row_4.str(), 7.f, 6.f);

	// Информация по продолжению игры или выходу
	mDrawHelper.drawText("Tap this banner to repeat", 7.f, 7.f);
	mDrawHelper.drawText("(or other area to exit)",   7.f, 8.f);
}

bool GameFinalStatistics::isMyAreaClicked(float x, float y) const {
	// Берем границы баннера и проверяем попадание x в них
	return (0.f < x - 7.f) && 
		   (x - 7.f < 7.f) && 
		   (y - 2.f < 7.f) && 
		   (0.f < y - 2.f);
}

GameControls::ControlsList& GameControls::getControls() {
	// Отдаем вчистую, даже можно менять
	// Вобщем, стандартный getter
	return mControls;
}

GameControls::ControlsList& GameControls::getClickableControls() {
	// аналогично предыдущему
	return mClickableControls;
}

void GameControls::addControl(GameControl * control) {
	// добавляем только ненулевые элементы
	if (control) { 
		mControls.push_back(control);
		// отслеживаем нажимабельные отдельно
		// чтобы потом не бегать не искать
		// копируем просто ссылку
		if (control->isClickable())
			mClickableControls.push_back(control);
	}
}

GameControls::~GameControls() {
	// удаляем из основного контейнера
	// все элементы управления
	while (!mControls.empty()) {
		delete mControls.back();
		mControls.pop_back();
	}
}

GameControl * GameControls::getControlByName(const std::string& objectName) {
	// Ищем первый попавшийся по названию
	for (GameControls::ControlsList::iterator control_iterator = mControls.begin();
		control_iterator != mControls.end();
		++control_iterator)
		if ((*control_iterator)->getObjectName().compare(objectName) == 0)
			return *control_iterator;
	return 0;
}

