#ifndef DRAWHELPER_H_INCLUDED
#define DRAWHELPER_H_INCLUDED

#include <string>
#include <map>

class CIw2DImage;
class CIw2DFont;
class CIwResGroup;
class ScaleMetrics;

// ��������������� ������� ��� ���������.
// ����� ������� ������ construct ����� ������������� Marmalade API!
// �������� ������� ��� ��������� � ������ �����������, � �� ����������
// �� ��������. ��� �� �� � �������! ��������� ����� destruct.
class DrawHelper {
public:

	// ������������ ����������
	typedef enum {
		PRIMITIVE_RECTANGLE
	} PRIMITIVE_TYPE;

	// �����
	typedef enum {
		COLOR_BLUE, COLOR_RED, COLOR_GREEN
	} COLOR;

	// �������������� ����������� ���������� �������
	// � ���������. ���������� ������ �������� � ������
	// �� ����� - ���������� ������ �������� ����
	// ���������� ��� ����������� ��������� ���������
	static void construct(float cellsX, float cellsY);
	
	// ������������ ������ resourceName.png
	// x, y, ������� - �� � ����������� �������� ����
	virtual void drawSprite(const std::string& resourceName, 
						float x, float y,
						float size_x = 1.f,
						float size_y = 1.f,	
						bool blending = false) const;

	// ������ ��������� ��������.
	// ���������� �������� ����.
	virtual void drawPrimitive(PRIMITIVE_TYPE primitiveType, 
                        float x, float y, float size_x, float size_y, COLOR color) const;

	// ������������ �����, � ����������� �������� ����
	virtual void drawText(const std::string& text, 
						  float x, float y) const;
	
	// ���������� � ������� ���������� ����������� �������
	static void destruct();

	// ��� ������ - ��������� �������� ����������
	// � ���������� �������� ����, � ��������
	static float getScreenX(float x);
	static float getScreenY(float y);
	
	static float getCellX(float x);
	static float getCellY(float y);

	static float getCellCenterX();
	static float getCellCenterY();

private:
	// �������, ����������� ��� ���������
	typedef std::map<std::string, CIw2DImage*> SpritesTable;
	static  CIw2DFont         * mFont;
	static  SpritesTable	  * mSprites;
	static  CIwResGroup       * mSpritesGroup;
	static  ScaleMetrics	  * mScaleMetrics;
};

#endif