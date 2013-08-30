#ifndef GAMEPROCESSOR_H_INCLUDED
#define GAMEPROCESSOR_H_INCLUDED

class GameRenderer;
class GameMode;

// �������� ����� ����������������� ����������.
// ������ ���� + ��������� ������� �� �������� ������.
// ������� �� ������ ���������� ��� � ������������ ������.
class GameProcessor {
	GameRenderer   * mGameRenderer;

public:
	// ��������� �������� �� ������
	// ��� ���� ����� ����� ������ �� �����������
	GameProcessor(GameRenderer& gameRenderer) : mGameRenderer(&gameRenderer),
	  mCurrentMode(0), mPreviousMode(0) {}

	// �������������: ������ ���������� ������ �����
	virtual void initialize(GameMode * firstMode);

	// �������� ������� ����
	virtual bool run();

	// ��������� ��������� �������. �������� �����������
	virtual void finalize();

protected:
	// ��������� ���� �� �������
	virtual void processStep(int time_delta);

	// ��������� ������� �� �����
	virtual void processTap(float x, float y);

	// ��������������� ����� ����� ������
	virtual void changeMode(GameMode * newMode);

	GameMode * mCurrentMode, * mPreviousMode;
};

#endif
