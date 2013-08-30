#ifndef DRAWHELPER_H_INCLUDED
#define DRAWHELPER_H_INCLUDED

#include <string>
#include <map>

class CIw2DImage;
class CIw2DFont;
class CIwResGroup;
class ScaleMetrics;

// Вспомогательные объекты для отрисовки.
// Класс требует вызова construct после инициализации Marmalade API!
// Содержит таблицу уже созданных в памяти изображений, и не подгружает
// их повторно. Сам их не и удаляет! Требуется вызов destruct.
class DrawHelper {
public:

	// Перечисление примитивов
	typedef enum {
		PRIMITIVE_RECTANGLE
	} PRIMITIVE_TYPE;

	// Цвета
	typedef enum {
		COLOR_BLUE, COLOR_RED, COLOR_GREEN
	} COLOR;

	// Подготавливает статические внутренние объекты
	// к отрисовке. Подгружает группы ресурсов и прочее
	// На входе - количество клеток игрового мира
	// Необходимо для последующих переводов координат
	static void construct(float cellsX, float cellsY);
	
	// Отрисовывает спрайт resourceName.png
	// x, y, размеры - всё в координатах игрового мира
	virtual void drawSprite(const std::string& resourceName, 
						float x, float y,
						float size_x = 1.f,
						float size_y = 1.f,	
						bool blending = false) const;

	// Рисует указанный примитив.
	// Координаты игрового мира.
	virtual void drawPrimitive(PRIMITIVE_TYPE primitiveType, 
                        float x, float y, float size_x, float size_y, COLOR color) const;

	// Отрисовывает текст, в координатах игрового мира
	virtual void drawText(const std::string& text, 
						  float x, float y) const;
	
	// Уничтожает и очищает внутренние статические объекты
	static void destruct();

	// Эти методы - переводят экранные координаты
	// в координаты игрового мира, и наоборот
	static float getScreenX(float x);
	static float getScreenY(float y);
	
	static float getCellX(float x);
	static float getCellY(float y);

	static float getCellCenterX();
	static float getCellCenterY();

private:
	// Объекты, необходимые для отрисовки
	typedef std::map<std::string, CIw2DImage*> SpritesTable;
	static  CIw2DFont         * mFont;
	static  SpritesTable	  * mSprites;
	static  CIwResGroup       * mSpritesGroup;
	static  ScaleMetrics	  * mScaleMetrics;
};

#endif