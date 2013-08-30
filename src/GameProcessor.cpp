#include "GameProcessor.h"
#include "GameRenderer.h"
#include "GameMode.h"
/*
#include "s3e.h"
#include "Iw2D.h"
*/
bool GameProcessor::run() {
	/*
		// Засечка времени старта цикла, и вычисляемая дельта, в мс
		int64 time_entry = s3eTimerGetMs(), time_delta = 0;

		// Необходимо отслеживать запросы на выход для их корректной обработки
		while(!s3eDeviceCheckQuitRequest()) {

			// Сперва обрабатываем нажатия на экран, так как они обновляют состояние игры
			if (s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_PRESSED)
				// Здесь передаются экранные координаты
				processTap(float(s3ePointerGetX()), float(s3ePointerGetY()));

			// Делаем шаг по времени для игрового мира.
			// Для большинства режимов здесь ничего не происходит
			processStep(int(time_delta));

			// Чистим экран белым
			Iw2DSurfaceClear(0xffffffff);

			// Отрисовываем всё и показываем
			mGameRenderer->renderBattleAndControls();
			Iw2DSurfaceShow();

			// После обработки событий нажатия
			// необходимо обновить состояние
			// в SDK
			s3ePointerUpdate();
			s3eKeyboardUpdate();

			// s3e-приложения должны передавать
			// управление ОС на минимальное время
			// постоянно, иначе нехорошо
			s3eDeviceYield();

			// Проверяем, запрашивает ли выход текущий режим
			if (mCurrentMode && mCurrentMode->isAtExit())
				break;

			// Вычисляем текущую дельту шага
			// То есть сколько времени прошло
			// за последнюю итерацию и обновляем засечку
			int64 current_time = s3eTimerGetMs();
			time_delta = current_time - time_entry;
			time_entry = current_time;
		}

		// Состояние режима может указывать на необходимость повтора игры
		// То есть если isAtRepeat - то игрок запросил повторить всё заново
		if (mCurrentMode && mCurrentMode->isAtRepeat())
			return true;
		else
			return false;

	*/

	return false;
}

void GameProcessor::initialize(GameMode * firstMode) {
	// Инициализируем внутренние переменные объекта
	mCurrentMode = firstMode;
	mPreviousMode = mCurrentMode;

	// Собственно нулевой режим - это тоже корректно
	// тогда будет просто отрисовка
	// ну а если режим задан, то его необходимо тоже
	// проинициализировать
	if (mCurrentMode)
		mCurrentMode->initialize();
}

void GameProcessor::finalize() {
	// Чистим память
	delete mCurrentMode;
}

void GameProcessor::changeMode(GameMode * newMode) {
	// Смена режимов сопровождается удалением предыдущего
	// Так как возвращаемые из режимов новые режимы, нигде
	// ни в каком контейнере не хранятся, и ими надо управлять
	// вручную. Но это несложно, как можно увидеть
	mCurrentMode = newMode;
	
	// Проверяем обновился ли режим
	if (mCurrentMode != mPreviousMode) {
		// Удаляем предыдущий
		delete mPreviousMode;
		mPreviousMode = mCurrentMode;

		// Не забываем проинициализировать новый
		mCurrentMode->initialize();
	}
}

void GameProcessor::processStep(int time_delta) {
	// За каждый шаг игрового мира отвечает текущий режим
	if (mCurrentMode)
		// передаем последнюю задержку в мс
		changeMode(mCurrentMode->processStep(time_delta));
}

void GameProcessor::processTap(float x, float y) {
	// За обработку пользовательский событий отвечает режим
	if (mCurrentMode)
		// передаем экранные координаты
		changeMode(mCurrentMode->processUserEvent(x, y));
}
