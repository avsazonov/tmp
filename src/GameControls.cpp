#include "GameControls.h"

using namespace TowerDefense;

const float gPlayButtonXFactor = 1.8f;

void PlayButton::render() const {
	// ������ ������ ������ ������
	if (int(getSetting("pause").getValue())) 
		mDrawHelper.drawSprite("start_btn", DrawHelper::getCellCenterX() * gPlayButtonXFactor, 0.f);
	else
		mDrawHelper.drawSprite("pause_btn", DrawHelper::getCellCenterX() * gPlayButtonXFactor, 0.f);
}

bool PlayButton::isMyAreaClicked(float x, float y) const {
	// ���������, ������ �� ������ � ������
	return (0.f < x - DrawHelper::getCellCenterX() * gPlayButtonXFactor) && 
		   // 1.f - ��� ������ ������ - ���� ������ 
		   (x - DrawHelper::getCellCenterX() * gPlayButtonXFactor < 1.f) && 
		   (y < 1.f)  && 
		   (0.f < y);
}

TowerChoiceMenu::TowerChoiceMenu() {
	// ������ ������ �� � � 7 ������
	setSetting("size_x", 7.f); 
	// �� y - � 3.5 ������
	setSetting("size_y", 3.5f); 

	// ���������� �� X
	setSetting("x", DrawHelper::getCellCenterX() - 3.5f);  
	// ���������� �� Y
	setSetting("y", DrawHelper::getCellCenterY() - 1.75f); 
}

void TowerChoiceMenu::render() const {
	// ������ ������ ����
	mDrawHelper.drawSprite("choice_menu", 
		getSetting("x").getValue(), 
		getSetting("y").getValue(), 
		getSetting("size_x").getValue(), 
		getSetting("size_y").getValue()
	);
}

bool TowerChoiceMenu::isMyAreaClicked(float x, float y) const {
	// ��������� ��������� � ������� ����
	return (0.f < x - getSetting("x").getValue()) && 
		   (x - getSetting("x").getValue() < getSetting("size_x").getValue())  && 
		   (y - getSetting("y").getValue() < getSetting("size_y").getValue())  && 
		   (0.f < y - getSetting("y").getValue());
}

#include <sstream>
void TimerBar::render() const {
	// ������ ����� m:ss
	std::stringstream text_stream;
	text_stream <<   int(getSetting("minutes").getValue()) << ":" 
	            << ((int(getSetting("seconds").getValue()) < 10) ? "0" : std::string())
	            <<   int(getSetting("seconds").getValue()) << std::flush;

	// ������������ � ����� ������� ���� �������� � ���� ������
	mDrawHelper.drawText(text_stream.str(), 0.f, 0.f);
}

void StatisticsBar::render() const {
	std::stringstream text_stream;
	text_stream << "Run out: " << int(getSetting("alive").getValue()) << "/5" << std::flush;

	// ������������ ����� � ����� ������ ����. 
	// ��� ����� ����� max y � �������� ������
	mDrawHelper.drawText(text_stream.str(), 0.f, DrawHelper::getCellCenterY() * 2 - 1.f);
}

void GameFinalStatistics::render() const {
	
	DrawHelper::COLOR bg_color;
	// � ����������� �� ����, ������� �� �����, �������� ���� �������
	bool win = getSetting("win").isSet() && (getSetting("win").getValue() > 0);
	
	if (win)
		bg_color = DrawHelper::COLOR_GREEN;
	else
		bg_color = DrawHelper::COLOR_RED;

	// ������ ��� �������
	// 7x5 ������, �� ������ ������ �� ���������
	mDrawHelper.drawPrimitive(DrawHelper::PRIMITIVE_RECTANGLE,
		                      7.f, 2.f, 7.f, 7.f, bg_color);

	// ������ ����� � ���������� � ����� ������
	if (win)
		mDrawHelper.drawText("You win!", 7.f, 2.f);
	else
		mDrawHelper.drawText("You lost", 7.f, 2.f);

	// ����� ����� ������� ���� ��� �������������
	// ���������� �������� ���������� �����
	// � ����������� �������� ��������� � �����

	// �� � ������ �������, ��� ����������� ������
	// ������� �� ����� �������, ������� ����� �����������

	std::stringstream row_1, row_2, row_3, row_4;
	// ������� ����� ������� ����
	row_1 << "Killed type 1: " << int(getSetting("killed_type_1").getValue()) << std::flush;
	// ������� �������
	row_2 << "Killed type 2: " << int(getSetting("killed_type_2").getValue()) << std::flush;
	// ������� ��������
	row_3 << "Killed type 3: " << int(getSetting("killed_type_3").getValue()) << std::flush;
	// � ������� �������
	row_4 << "Run out: " << int(getSetting("alive").getValue()) << std::flush;
	
	mDrawHelper.drawText(row_1.str(), 7.f, 3.f);
	mDrawHelper.drawText(row_2.str(), 7.f, 4.f);
	mDrawHelper.drawText(row_3.str(), 7.f, 5.f);
	mDrawHelper.drawText(row_4.str(), 7.f, 6.f);

	// ���������� �� ����������� ���� ��� ������
	mDrawHelper.drawText("Tap this banner to repeat", 7.f, 7.f);
	mDrawHelper.drawText("(or other area to exit)",   7.f, 8.f);
}

bool GameFinalStatistics::isMyAreaClicked(float x, float y) const {
	// ����� ������� ������� � ��������� ��������� x � ���
	return (0.f < x - 7.f) && 
		   (x - 7.f < 7.f) && 
		   (y - 2.f < 7.f) && 
		   (0.f < y - 2.f);
}

GameControls::ControlsList& GameControls::getControls() {
	// ������ �������, ���� ����� ������
	// ������, ����������� getter
	return mControls;
}

GameControls::ControlsList& GameControls::getClickableControls() {
	// ���������� �����������
	return mClickableControls;
}

void GameControls::addControl(GameControl * control) {
	// ��������� ������ ��������� ��������
	if (control) { 
		mControls.push_back(control);
		// ����������� ������������� ��������
		// ����� ����� �� ������ �� ������
		// �������� ������ ������
		if (control->isClickable())
			mClickableControls.push_back(control);
	}
}

GameControls::~GameControls() {
	// ������� �� ��������� ����������
	// ��� �������� ����������
	while (!mControls.empty()) {
		delete mControls.back();
		mControls.pop_back();
	}
}

GameControl * GameControls::getControlByName(const std::string& objectName) {
	// ���� ������ ���������� �� ��������
	for (GameControls::ControlsList::iterator control_iterator = mControls.begin();
		control_iterator != mControls.end();
		++control_iterator)
		if ((*control_iterator)->getObjectName().compare(objectName) == 0)
			return *control_iterator;
	return 0;
}

