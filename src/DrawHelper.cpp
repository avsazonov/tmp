#include "DrawHelper.h"
//#include "Iw2D.h"
//#include "IwResManager.h"

// Класс, объекты которого отвечают за перевод координат 
// игрового мира в координаты дисплея и наоборот
// Скрыт от остальных, поскольку должен быть нужен только
// классу DrawHelper, который выдаёт интерфейс к нему
class ScaleMetrics {
	float mCellsX;
	float mCellsY;
	float mScreenX;
	float mScreenY;
public:
	// По порядку: x и y игровых клеток, и x и y экранные, максимальные значения
	ScaleMetrics(float cellsX, float cellsY, float screenX, float screenY) :
	  mCellsX(cellsX), mCellsY(cellsY), mScreenX(screenX), mScreenY(screenY) {}

	// Перевод координат игровых в экранные
	float getScreenX(float cellX) { return mScreenX * cellX / mCellsX; }
	float getScreenY(float cellY) { return mScreenY * cellY / mCellsY; }

	// Перевод экранных координат в игровые
	float getCellX(float screenX) { return mCellsX * screenX / mScreenX; }
	float getCellY(float screenY) { return mCellsY * screenY / mScreenY; }

	// Центр поля в игровых координатах
	float getCellCenterX() { return mCellsX / 2; }
	float getCellCenterY() { return mCellsY / 2; }
};

// Интерфейс к переводу координат
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
	// Создаем, если не создан, объект перевода координат
	if (!mScaleMetrics) {
		mScaleMetrics = new ScaleMetrics(cellsX, cellsY, float(Iw2DGetSurfaceWidth()), float(Iw2DGetSurfaceHeight()));
	}

	// Создаем таблицу спрайтов
	if (!mSprites)
		mSprites = new DrawHelper::SpritesTable;

	// Подгружаем группу ресурсов спрайтов
	if (!mSpritesGroup) {
		IwGetResManager()->LoadGroup("sprites.group");
		mSpritesGroup = IwGetResManager()->GetGroupNamed("sprites");
		IwGetResManager()->SetCurrentGroup(mSpritesGroup);
	}

	// И шрифт
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

	// Ищем сперва спрайт в таблице уже подгруженных из ресурсов
	// Вообще говоря, Iw2D делает сам batching, но не для класса CIw2DImage
	// объекты которого мы менеджим сами
	DrawHelper::SpritesTable::iterator sprite_index = mSprites->find(resourceName);
	if (mSprites->end() != sprite_index) 
		sprite = sprite_index->second;
	else {
		// если не нашли - подгружаем
		sprite = Iw2DCreateImageResource(resourceName.c_str());
		// не смогли - выходим
		if (!sprite)
			return;
		// подгрузили - добавляем в таблицу
		(*mSprites)[resourceName] = sprite;
	}

	if (blending)
		Iw2DSetAlphaMode(IW_2D_ALPHA_HALF);

	// Рисуем картинку!
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

	// Запоминаем текущий цвет, чтобы потом восстановить
	CIwColour current_colour = Iw2DGetColour();

	// Ставим запрошенный цвет
	if (DrawHelper::COLOR_BLUE == color)
		Iw2DSetColour(0xffff0000); // ABGR format!
	else if (DrawHelper::COLOR_RED == color)
		Iw2DSetColour(0xff0000ff); // ABGR format!
	else if (DrawHelper::COLOR_GREEN == color)
		Iw2DSetColour(0xff00ff00); // ABGR format!

	// Рисуем прямоугольник
	if (primitiveType == DrawHelper::PRIMITIVE_RECTANGLE)
		Iw2DFillRect(CIwFVec2(mScaleMetrics->getScreenX(x), mScaleMetrics->getScreenY(y)),
					 CIwFVec2(mScaleMetrics->getScreenX(size_x), mScaleMetrics->getScreenY(size_y)));

	// Восстанавливаем цвет
	Iw2DSetColour(current_colour); 
	*/
}

void DrawHelper::drawText(const std::string& text, float x, float y) const {
	/*
	// Шрифта нет - ничего нет
	if (!mFont)
		return;

	Iw2DSetFont(mFont);
	
	// Запоминаем цвет
	CIwColour current_colour = Iw2DGetColour();
	
	// Пишем всегда черный текст
	Iw2DSetColour(0xff000000);

	// Отображаем строку по всей её длине, а по высоте - всегда полклетки игрового мира
	// Центрируем текст
	const float font_cell_y = 0.5f;
	Iw2DDrawString(text.c_str(), CIwFVec2(mScaleMetrics->getScreenX(x), mScaleMetrics->getScreenY(y)),
				  CIwFVec2(float(Iw2DGetStringWidth(text.c_str())), mScaleMetrics->getScreenX(font_cell_y)), 
	              IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

	// восстанавливаем цвет
	Iw2DSetColour(current_colour);
	*/
}

void DrawHelper::destruct() {
	/*
	// Чистим таблицу созданных изображений
	for (DrawHelper::SpritesTable::iterator entry = mSprites->begin();
		entry != mSprites->end();
		++entry)
		delete entry->second;
	
	// И саму таблицу удаляем
	delete mSprites; 
	// Ставим 0 на тот случай
	// если еще раз запустят construct
	mSprites = 0;
	
	// Объект перевода координат тоже чистим
	delete mScaleMetrics; 
	mScaleMetrics = 0;
	
	// И шрифт
	delete mFont; 
	mFont = 0;

	// Группу не удаляем, SDK не требует
	// Да и при перезапуске будет нужна
	// та же группа
	*/
}
