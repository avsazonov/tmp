#include "DrawHelper.h"
//#include "Iw2D.h"
//#include "IwResManager.h"

// �����, ������� �������� �������� �� ������� ��������� 
// �������� ���� � ���������� ������� � ��������
// ����� �� ���������, ��������� ������ ���� ����� ������
// ������ DrawHelper, ������� ����� ��������� � ����
class ScaleMetrics {
	float mCellsX;
	float mCellsY;
	float mScreenX;
	float mScreenY;
public:
	// �� �������: x � y ������� ������, � x � y ��������, ������������ ��������
	ScaleMetrics(float cellsX, float cellsY, float screenX, float screenY) :
	  mCellsX(cellsX), mCellsY(cellsY), mScreenX(screenX), mScreenY(screenY) {}

	// ������� ��������� ������� � ��������
	float getScreenX(float cellX) { return mScreenX * cellX / mCellsX; }
	float getScreenY(float cellY) { return mScreenY * cellY / mCellsY; }

	// ������� �������� ��������� � �������
	float getCellX(float screenX) { return mCellsX * screenX / mScreenX; }
	float getCellY(float screenY) { return mCellsY * screenY / mScreenY; }

	// ����� ���� � ������� �����������
	float getCellCenterX() { return mCellsX / 2; }
	float getCellCenterY() { return mCellsY / 2; }
};

// ��������� � �������� ���������
float DrawHelper::getScreenX(float x) {
	return mScaleMetrics->getScreenX(x);
}

float DrawHelper::getScreenY(float y) {
	return mScaleMetrics->getScreenY(y);
}
	
float DrawHelper::getCellX(float x) {
	return mScaleMetrics->getCellX(x);
}

float DrawHelper::getCellY(float y) {
	return mScaleMetrics->getCellY(y);
}

float DrawHelper::getCellCenterX() {
	return mScaleMetrics->getCellCenterX();
}

float DrawHelper::getCellCenterY() {
	return mScaleMetrics->getCellCenterY();
}

CIwResGroup              * DrawHelper::mSpritesGroup = 0;
DrawHelper::SpritesTable * DrawHelper::mSprites      = 0;
ScaleMetrics             * DrawHelper::mScaleMetrics = 0;
CIw2DFont                * DrawHelper::mFont         = 0;

void DrawHelper::construct(float cellsX, float cellsY) {
	/*
	// �������, ���� �� ������, ������ �������� ���������
	if (!mScaleMetrics) {
		mScaleMetrics = new ScaleMetrics(cellsX, cellsY, float(Iw2DGetSurfaceWidth()), float(Iw2DGetSurfaceHeight()));
	}

	// ������� ������� ��������
	if (!mSprites)
		mSprites = new DrawHelper::SpritesTable;

	// ���������� ������ �������� ��������
	if (!mSpritesGroup) {
		IwGetResManager()->LoadGroup("sprites.group");
		mSpritesGroup = IwGetResManager()->GetGroupNamed("sprites");
		IwGetResManager()->SetCurrentGroup(mSpritesGroup);
	}

	// � �����
	if (!mFont) {
		IwGetResManager()->SetCurrentGroup(mSpritesGroup);
		mFont = Iw2DCreateFontResource("font");
	}
	*/
}

void DrawHelper::drawSprite(const std::string& resourceName, float x, float y, float size_x, float size_y, bool blending) const {
	/*
	IwGetResManager()->SetCurrentGroup(mSpritesGroup);

	CIw2DImage * sprite = 0;

	// ���� ������ ������ � ������� ��� ������������ �� ��������
	// ������ ������, Iw2D ������ ��� batching, �� �� ��� ������ CIw2DImage
	// ������� �������� �� �������� ����
	DrawHelper::SpritesTable::iterator sprite_index = mSprites->find(resourceName);
	if (mSprites->end() != sprite_index) 
		sprite = sprite_index->second;
	else {
		// ���� �� ����� - ����������
		sprite = Iw2DCreateImageResource(resourceName.c_str());
		// �� ������ - �������
		if (!sprite)
			return;
		// ���������� - ��������� � �������
		(*mSprites)[resourceName] = sprite;
	}

	if (blending)
		Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);

	// ������ ��������!
	Iw2DDrawImage(sprite, 
		CIwFVec2(mScaleMetrics->getScreenX(x),
		         mScaleMetrics->getScreenY(y)), 
		CIwFVec2(mScaleMetrics->getScreenX(size_x), 
				 mScaleMetrics->getScreenY(size_y))
	);

	if (blending)
		Iw2DSetAlphaMode(IW_2D_ALPHA_NONE);
	*/
}

void DrawHelper::drawPrimitive(DrawHelper::PRIMITIVE_TYPE primitiveType, 
	float x, float y, float size_x, float size_y, DrawHelper::COLOR color) const {

	/*

	// ���������� ������� ����, ����� ����� ������������
	CIwColour current_colour = Iw2DGetColour();

	// ������ ����������� ����
	if (DrawHelper::COLOR_BLUE == color)
		Iw2DSetColour(0xffff0000); // ABGR format!
	else if (DrawHelper::COLOR_RED == color)
		Iw2DSetColour(0xff0000ff); // ABGR format!
	else if (DrawHelper::COLOR_GREEN == color)
		Iw2DSetColour(0xff00ff00); // ABGR format!

	// ������ �������������
	if (primitiveType == DrawHelper::PRIMITIVE_RECTANGLE)
		Iw2DFillRect(CIwFVec2(mScaleMetrics->getScreenX(x), mScaleMetrics->getScreenY(y)),
					 CIwFVec2(mScaleMetrics->getScreenX(size_x), mScaleMetrics->getScreenY(size_y)));

	// ��������������� ����
	Iw2DSetColour(current_colour); 
	*/
}

void DrawHelper::drawText(const std::string& text, float x, float y) const {
	/*
	// ������ ��� - ������ ���
	if (!mFont)
		return;

	Iw2DSetFont(mFont);
	
	// ���������� ����
	CIwColour current_colour = Iw2DGetColour();
	
	// ����� ������ ������ �����
	Iw2DSetColour(0xff000000);

	// ���������� ������ �� ���� � �����, � �� ������ - ������ ��������� �������� ����
	// ���������� �����
	const float font_cell_y = 0.5f;
	Iw2DDrawString(text.c_str(), CIwFVec2(mScaleMetrics->getScreenX(x), mScaleMetrics->getScreenY(y)),
				  CIwFVec2(float(Iw2DGetStringWidth(text.c_str())), mScaleMetrics->getScreenX(font_cell_y)), 
	              IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	// ��������������� ����
	Iw2DSetColour(current_colour);
	*/
}

void DrawHelper::destruct() {
	/*
	// ������ ������� ��������� �����������
	for (DrawHelper::SpritesTable::iterator entry = mSprites->begin();
		entry != mSprites->end();
		++entry)
		delete entry->second;
	
	// � ���� ������� �������
	delete mSprites; 
	// ������ 0 �� ��� ������
	// ���� ��� ��� �������� construct
	mSprites = 0;
	
	// ������ �������� ��������� ���� ������
	delete mScaleMetrics; 
	mScaleMetrics = 0;
	
	// � �����
	delete mFont; 
	mFont = 0;

	// ������ �� �������, SDK �� �������
	// �� � ��� ����������� ����� �����
	// �� �� ������
	*/
}
