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

// Нужны для инициализации random
#include <cstdlib>
#include <ctime>

// Отрисовывает две картинки из ресурсов.
// splash1.png, splash2.png.
// Требует инициализированных Iw2D и ResManager
void drawSplashes() {
	// Время показа каждого сплэша
	const int time = 100; // мс

	// Загрузка группы ресурсов со сплэшами
	IwGetResManager()->LoadGroup("splashes.group");

	// Подгрузка изображений
	CIw2DImage * splash_one = Iw2DCreateImageResource("splash1");
	CIw2DImage * splash_two = Iw2DCreateImageResource("splash2");

	// Здесь нет проверки на нулевой указатель; 
	// если картинок нет в ресурсах, значит приложение сломано.

	// Проверка показа первой картинки
	bool first_splash = true;
	// Счетчик времени показа
	int  time_counter = 0;

	// Центрируем (предполагая, что обе картинки одинакового размера)
	CIwFVec2 position(
		Iw2DGetSurfaceWidth()/2 - splash_one->GetWidth()/2,
		Iw2DGetSurfaceHeight()/2 - splash_one->GetHeight()/2
	);

	// Цикл обработки сообщений о выходе
    while(!s3eDeviceCheckQuitRequest()) {

		// Очистка экрана белым
		Iw2DSurfaceClear(0xffffffff);

		// Рисование
		Iw2DDrawImage(first_splash ? splash_one : splash_two, position);		
		Iw2DSurfaceShow();
		
		// Ждём 1 мс
		s3eDeviceYield(1);
		++time_counter;

		// Меняем изображения или выходим из цикла
		if (0 == time_counter % time) {
			if (!first_splash)
				break;
			time_counter = 0;
			first_splash = false;
		}
	}

	// Подчищаем за собой
	delete splash_one;
	delete splash_two;
}

// "main" - входная точка приложений s3e
int main()
{

	// Сеем генератор случайных чисел
	srand(time(0));

	// Инициализируем библиотеки SDK
	Iw2DInit();
	IwResManagerInit();
	
	// Рисуем сплэши
	drawSplashes();

	// Повторяем ли игру
	bool repeat_game = true;

#ifdef DEBUG_SETTINGS_CHECK
	// отладочный блок
	// когда включен
	// выводит в консоль
	// сообщения об ошибках
	try {
#endif
		
		while (repeat_game)
		// Скобки необходимы всегда для корректной работы STL!
		// То есть если убирать while, скобки необходимо оставить,
		// для того чтобы игровые объекты уничтожались раньше, чем
		// будут закрыты библиотеки SDK
		{
			// Создаем игровое поле
			BattleField battleField;
			WorldCreator worldCreator(battleField);
			worldCreator.create();

			// Создаем контейнер для элементов управления в игре
			GameControls gameControls;

			// Создаем рендерер этих объектов
			GameRenderer gameRenderer(battleField, gameControls);

			// Создаем "процессор" игрового мира, отвечающий за режим атаки
			WorldProcessor worldProcessor(worldCreator, battleField);

			// Подготавливаем все объекты, связанные с отрисовкой объектов игры
			// Устанавливаем значения, равные X и Y в клетках для игрового мира
			DrawHelper::construct(worldCreator.getCellsX(), worldCreator.getCellsY());

			// Создаем и инициализируем сам объект игры (уровень)
			// Первый режим - режим расстановки башен
			// Остальные будут меняться автоматически
			// (в зависимости от элементов управления)
			GameProcessor game(gameRenderer);
			game.initialize(new TowerDefense::PreparingFieldMode(battleField, gameControls, worldCreator, worldProcessor));
		
			// Запускаем игровой цикл
			repeat_game = game.run();

			// Подчищаем всё, что осталось
			game.finalize();
			DrawHelper::destruct();
		}
#ifdef DEBUG_SETTINGS_CHECK
	// собственно отлов исключений и вывод
	} catch (const std::exception& exception) {
		s3eDebugOutputString(exception.what());
	}
#endif

	// Завершаем библиотеки
	IwResManagerTerminate();
    Iw2DTerminate();

    return 0;
}
