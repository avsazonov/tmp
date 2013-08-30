#include "s3e.h"
#include "Iw2D.h"
#include "IwResManager.h"
#include "IwGeom.h"

#include "GameRenderer.h"
#include "GameControls.h"
#include "WorldCreator.h"
#include "WorldProcessor.h"
#include "GameMode.h"
#include "GameProcessor.h"
#include "FieldUnit.h"

// ����� ��� ������������� random
#include <cstdlib>
#include <ctime>

// ������������ ��� �������� �� ��������.
// splash1.png, splash2.png.
// ������� ������������������ Iw2D � ResManager
void drawSplashes() {
	// ����� ������ ������� ������
	const int time = 100; // ��

	// �������� ������ �������� �� ��������
	IwGetResManager()->LoadGroup("splashes.group");

	// ��������� �����������
	CIw2DImage * splash_one = Iw2DCreateImageResource("splash1");
	CIw2DImage * splash_two = Iw2DCreateImageResource("splash2");

	// ����� ��� �������� �� ������� ���������; 
	// ���� �������� ��� � ��������, ������ ���������� �������.

	// �������� ������ ������ ��������
	bool first_splash = true;
	// ������� ������� ������
	int  time_counter = 0;

	// ���������� (�����������, ��� ��� �������� ����������� �������)
	CIwFVec2 position(
		Iw2DGetSurfaceWidth()/2 - splash_one->GetWidth()/2,
		Iw2DGetSurfaceHeight()/2 - splash_one->GetHeight()/2
	);

	// ���� ��������� ��������� � ������
    while(!s3eDeviceCheckQuitRequest()) {

		// ������� ������ �����
		Iw2DSurfaceClear(0xffffffff);

		// ���������
		Iw2DDrawImage(first_splash ? splash_one : splash_two, position);		
		Iw2DSurfaceShow();
		
		// ��� 1 ��
		s3eDeviceYield(1);
		++time_counter;

		// ������ ����������� ��� ������� �� �����
		if (0 == time_counter % time) {
			if (!first_splash)
				break;
			time_counter = 0;
			first_splash = false;
		}
	}

	// ��������� �� �����
	delete splash_one;
	delete splash_two;
}

// "main" - ������� ����� ���������� s3e
int main()
{

	// ���� ��������� ��������� �����
	srand(time(0));

	// �������������� ���������� SDK
	Iw2DInit();
	IwResManagerInit();
	
	// ������ ������
	drawSplashes();

	// ��������� �� ����
	bool repeat_game = true;

#ifdef DEBUG_SETTINGS_CHECK
	// ���������� ����
	// ����� �������
	// ������� � �������
	// ��������� �� �������
	try {
#endif
		
		while (repeat_game)
		// ������ ���������� ������ ��� ���������� ������ STL!
		// �� ���� ���� ������� while, ������ ���������� ��������,
		// ��� ���� ����� ������� ������� ������������ ������, ���
		// ����� ������� ���������� SDK
		{
			// ������� ������� ����
			BattleField battleField;
			WorldCreator worldCreator(battleField);
			worldCreator.create();

			// ������� ��������� ��� ��������� ���������� � ����
			GameControls gameControls;

			// ������� �������� ���� ��������
			GameRenderer gameRenderer(battleField, gameControls);

			// ������� "���������" �������� ����, ���������� �� ����� �����
			WorldProcessor worldProcessor(worldCreator, battleField);

			// �������������� ��� �������, ��������� � ���������� �������� ����
			// ������������� ��������, ������ X � Y � ������� ��� �������� ����
			DrawHelper::construct(worldCreator.getCellsX(), worldCreator.getCellsY());

			// ������� � �������������� ��� ������ ���� (�������)
			// ������ ����� - ����� ����������� �����
			// ��������� ����� �������� �������������
			// (� ����������� �� ��������� ����������)
			GameProcessor game(gameRenderer);
			game.initialize(new TowerDefense::PreparingFieldMode(battleField, gameControls, worldCreator, worldProcessor));
		
			// ��������� ������� ����
			repeat_game = game.run();

			// ��������� ��, ��� ��������
			game.finalize();
			DrawHelper::destruct();
		}
#ifdef DEBUG_SETTINGS_CHECK
	// ���������� ����� ���������� � �����
	} catch (const std::exception& exception) {
		s3eDebugOutputString(exception.what());
	}
#endif

	// ��������� ����������
	IwResManagerTerminate();
    Iw2DTerminate();

    return 0;
}
