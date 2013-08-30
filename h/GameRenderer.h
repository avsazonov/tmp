#ifndef GAMERENDERER_H_INCLUDED
#define GAMERENDERER_H_INCLUDED

class BattleField;
class GameControls;

// ������� ����� ������ �������� �� ���� �����
// ����������� � � ������������ c ��������.
// ����� ������������ ������� ���������� �����.
class GameRenderer {

	BattleField  * mBattleField;
	GameControls * mControls;

public:
	// ���������������� �� �������
	GameRenderer(BattleField& battleField, GameControls& controls) : 
	  mBattleField(&battleField), mControls(&controls) {}

	// ����������� ����� ���������� ��
	// ������ �� ������ � �� ������ (� ������ �����)
	// ������ ���, � ��������, �� ����� ���� ���������
	virtual void renderBattleAndControls() const;
};

#endif
