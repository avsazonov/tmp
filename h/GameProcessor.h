#ifndef GAMEPROCESSOR_H_INCLUDED
#define GAMEPROCESSOR_H_INCLUDED

class GameRenderer;
class GameMode;

// Основной класс пользовательского интерфейса.
// Режимы игры + обработка реакций на действия игрока.
// Удаляет из памяти переданные ему в распоряжение режимы.
class GameProcessor {
	GameRenderer   * mGameRenderer;

public:
	// Принимаем рендерер по ссылке
	// Для того чтобы битые ссылки не вставлялись
	GameProcessor(GameRenderer& gameRenderer) : mGameRenderer(&gameRenderer),
	  mCurrentMode(0), mPreviousMode(0) {}

	// Инициализация: должен задаваться первый режим
	virtual void initialize(GameMode * firstMode);

	// Основной игровой цикл
	virtual bool run();

	// Финальные процедуры очистки. Вызывать обязательно
	virtual void finalize();

protected:
	// Обработка шага по времени
	virtual void processStep(int time_delta);

	// Обработка нажатия на экран
	virtual void processTap(float x, float y);

	// Вспомогательный метод смены режима
	virtual void changeMode(GameMode * newMode);

	GameMode * mCurrentMode, * mPreviousMode;
};

#endif
