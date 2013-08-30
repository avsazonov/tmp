#ifndef GAMERENDERER_H_INCLUDED
#define GAMERENDERER_H_INCLUDED

class BattleField;
class GameControls;

// Объекты этого класса проходят по всей карте
// отрисовывая её в соответствии c уровнями.
// Также отрисовывает объекты управления игрой.
class GameRenderer {

	BattleField  * mBattleField;
	GameControls * mControls;

public:
	// Инициализируемся по ссылкам
	GameRenderer(BattleField& battleField, GameControls& controls) : 
	  mBattleField(&battleField), mControls(&controls) {}

	// Виртуальный метод отрисовать всё
	// Ничего не чистит и не флашит (в смысле экран)
	// Потому что, в принципе, их может быть несколько
	virtual void renderBattleAndControls() const;
};

#endif
