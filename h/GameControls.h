#ifndef GAMECONTROLS_H_INCLUDED
#define GAMECONTROLS_H_INCLUDED

#include "GameObject.h"
#include <list>

// Абстрактный элемент управления игрой
// Содержит признак активности
class GameControl : public GameObject, public Renderable {
	bool mEnabled;
public:
	// По умолчанию неактивен
	GameControl() : mEnabled(false) {}

	// Активировать
	void enable() { mEnabled = true; }

	// Активен ли?
	bool isEnabled() { return mEnabled; }

	// Сделать неактивным
	void disable()  { mEnabled = false; }
};

namespace TowerDefense {

	// Кнопка остановки/продолжения игры
	// Содержит настройку pause, которая
	// отвечает за состояние элемента, и
	// соответственно за отрисовку
	class PlayButton : public GameControl {
	public:
		// По умолчанию "процесс не запущен"
		PlayButton() { setSetting("pause", 1.f);	}
		void render() const;

		std::string getObjectName() const { return "PlayButton"; }
		
		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	};

	// Меню выбора типа башни
	// Содержит настройки x, y, size_x, size_y
	// Объекты управляются с ними сами
	class TowerChoiceMenu : public GameControl {
	public:
		TowerChoiceMenu();
		void render() const;
		std::string getObjectName() const { return "TowerChoiceMenu"; }

		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	};

	// Вывод прошедшего времени
	// Требует установки двух настроек:
	// minutes и seconds для вывода
	class TimerBar : public GameControl {
	public:
		void render() const;
		std::string getObjectName() const { return "TimerBar"; }
	};

	// Вывод статистики убежавших юнитов
	// Требует одну настройку alive
	class StatisticsBar : public GameControl {
	public:
		void render() const;
		std::string getObjectName() const { return "StatisticsBar"; }
	};

	// Вывод финального баннера
	// Требует установленных настроек
	// win, alive, killed_type_1, killed_type_2, killed_type_3
	class GameFinalStatistics : public GameControl {
	public: 
		void render() const;
		std::string getObjectName() const { return "GameFinalStatistics"; }
		
		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	};
}

// Контейнер элементов управления в игре
// Сам удаляет все объекты! Не создавать элементы управления на стеке!
class GameControls {
public:
	typedef std::list<GameControl*> ControlsList;

	// Добавление элемента управления в контейнер
	void addControl(GameControl * control);

	// Методы получения элементов управления
	ControlsList& getControls();
	ControlsList& getClickableControls();
	// Возвращает первый с данным именем
	GameControl * getControlByName(const std::string &objectName);
	
	// Всё удаляет
	virtual ~GameControls();
private:
	ControlsList mControls;
	ControlsList mClickableControls;
};

#endif
