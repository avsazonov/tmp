#include "GameProcessor.h"
#include "GameRenderer.h"
#include "GameMode.h"
/*
#include "s3e.h"
#include "Iw2D.h"
*/
bool GameProcessor::run() {
	/*
		// ������� ������� ������ �����, � ����������� ������, � ��
		int64 time_entry = s3eTimerGetMs(), time_delta = 0;

		// ���������� ����������� ������� �� ����� ��� �� ���������� ���������
		while(!s3eDeviceCheckQuitRequest()) {

			// ������ ������������ ������� �� �����, ��� ��� ��� ��������� ��������� ����
			if (s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_PRESSED)
				// ����� ���������� �������� ����������
				processTap(float(s3ePointerGetX()), float(s3ePointerGetY()));

			// ������ ��� �� ������� ��� �������� ����.
			// ��� ����������� ������� ����� ������ �� ����������
			processStep(int(time_delta));

			// ������ ����� �����
			Iw2DSurfaceClear(0xffffffff);

			// ������������ �� � ����������
			mGameRenderer->renderBattleAndControls();
			Iw2DSurfaceShow();

			// ����� ��������� ������� �������
			// ���������� �������� ���������
			// � SDK
			s3ePointerUpdate();
			s3eKeyboardUpdate();

			// s3e-���������� ������ ����������
			// ���������� �� �� ����������� �����
			// ���������, ����� ��������
			s3eDeviceYield();

			// ���������, ����������� �� ����� ������� �����
			if (mCurrentMode && mCurrentMode->isAtExit())
				break;

			// ��������� ������� ������ ����
			// �� ���� ������� ������� ������
			// �� ��������� �������� � ��������� �������
			int64 current_time = s3eTimerGetMs();
			time_delta = current_time - time_entry;
			time_entry = current_time;
		}

		// ��������� ������ ����� ��������� �� ������������� ������� ����
		// �� ���� ���� isAtRepeat - �� ����� �������� ��������� �� ������
		if (mCurrentMode && mCurrentMode->isAtRepeat())
			return true;
		else
			return false;

	*/

	return false;
}

void GameProcessor::initialize(GameMode * firstMode) {
	// �������������� ���������� ���������� �������
	mCurrentMode = firstMode;
	mPreviousMode = mCurrentMode;

	// ���������� ������� ����� - ��� ���� ���������
	// ����� ����� ������ ���������
	// �� � ���� ����� �����, �� ��� ���������� ����
	// �������������������
	if (mCurrentMode)
		mCurrentMode->initialize();
}

void GameProcessor::finalize() {
	// ������ ������
	delete mCurrentMode;
}

void GameProcessor::changeMode(GameMode * newMode) {
	// ����� ������� �������������� ��������� �����������
	// ��� ��� ������������ �� ������� ����� ������, �����
	// �� � ����� ���������� �� ��������, � ��� ���� ���������
	// �������. �� ��� ��������, ��� ����� �������
	mCurrentMode = newMode;
	
	// ��������� ��������� �� �����
	if (mCurrentMode != mPreviousMode) {
		// ������� ����������
		delete mPreviousMode;
		mPreviousMode = mCurrentMode;

		// �� �������� ������������������� �����
		mCurrentMode->initialize();
	}
}

void GameProcessor::processStep(int time_delta) {
	// �� ������ ��� �������� ���� �������� ������� �����
	if (mCurrentMode)
		// �������� ��������� �������� � ��
		changeMode(mCurrentMode->processStep(time_delta));
}

void GameProcessor::processTap(float x, float y) {
	// �� ��������� ���������������� ������� �������� �����
	if (mCurrentMode)
		// �������� �������� ����������
		changeMode(mCurrentMode->processUserEvent(x, y));
}
