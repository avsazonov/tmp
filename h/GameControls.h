#ifndef GAMECONTROLS_H_INCLUDED
#define GAMECONTROLS_H_INCLUDED

#include "GameObject.h"
#include <list>

// ����������� ������� ���������� �����
// �������� ������� ����������
class GameControl : public GameObject, public Renderable {
	bool mEnabled;
public:
	// �� ��������� ���������
	GameControl() : mEnabled(false) {}

	// ������������
	void enable() { mEnabled = true; }

	// ������� ��?
	bool isEnabled() { return mEnabled; }

	// ������� ����������
	void disable()  { mEnabled = false; }
};

namespace TowerDefense {

	// ������ ���������/����������� ����
	// �������� ��������� pause, �������
	// �������� �� ��������� ��������, �
	// �������������� �� ���������
	class PlayButton : public GameControl {
	public:
		// �� ��������� "������� �� �������"
		PlayButton() { setSetting("pause", 1.f);	}
		void render() const;

		std::string getObjectName() const { return "PlayButton"; }
		
		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	};

	// ���� ������ ���� �����
	// �������� ��������� x, y, size_x, size_y
	// ������� ����������� � ���� ����
	class TowerChoiceMenu : public GameControl {
	public:
		TowerChoiceMenu();
		void render() const;
		std::string getObjectName() const { return "TowerChoiceMenu"; }

		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	};

	// ����� ���������� �������
	// ������� ��������� ���� ��������:
	// minutes � seconds ��� ������
	class TimerBar : public GameControl {
	public:
		void render() const;
		std::string getObjectName() const { return "TimerBar"; }
	};

	// ����� ���������� ��������� ������
	// ������� ���� ��������� alive
	class StatisticsBar : public GameControl {
	public:
		void render() const;
		std::string getObjectName() const { return "StatisticsBar"; }
	};

	// ����� ���������� �������
	// ������� ������������� ��������
	// win, alive, killed_type_1, killed_type_2, killed_type_3
	class GameFinalStatistics : public GameControl {
	public: 
		void render() const;
		std::string getObjectName() const { return "GameFinalStatistics"; }
		
		bool isClickable() const { return true; }
		bool isMyAreaClicked(float x, float y) const;
	};
}

// ��������� ��������� ���������� � ����
// ��� ������� ��� �������! �� ��������� �������� ���������� �� �����!
class GameControls {
public:
	typedef std::list<GameControl*> ControlsList;

	// ���������� �������� ���������� � ���������
	void addControl(GameControl * control);

	// ������ ��������� ��������� ����������
	ControlsList& getControls();
	ControlsList& getClickableControls();
	// ���������� ������ � ������ ������
	GameControl * getControlByName(const std::string &objectName);
	
	// �� �������
	virtual ~GameControls();
private:
	ControlsList mControls;
	ControlsList mClickableControls;
};

#endif
