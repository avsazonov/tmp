#include "WorldCreator.h"
#include "WorldProcessor.h"
#include "FieldUnit.h"
#include "LayerConstants.h"

#include <cmath>
#include <cstdlib>

Randomizer::UnitsList Randomizer::createRandomUnits(int timeDelta) {

	mTimeCounter += float(timeDelta);

	// ������� ��� ������������ �������� ������ ���������
	Randomizer::UnitsList result;

	if ((mTimeCounter - mLastCreation) > mRandomTime) {

		// �������� �� ���� ������ �� ����� � ������� ����� ���������� ����
		for (WorldCreator::PointsList::const_iterator entry_iterator = mWorldCreator->getMapEntries().begin();
			entry_iterator != mWorldCreator->getMapEntries().end();
			++entry_iterator) {

			int enemy_type = rand() % 3;

			// ������� �����
			TowerDefense::Enemy * enemy = mWorldCreator->createEnemy(WorldCreator::ENEMYTYPE(enemy_type));
			if (enemy) {
				enemy->setSetting("x", entry_iterator->first);
				enemy->setSetting("y", entry_iterator->second);
				enemy->setSetting("entry_x", entry_iterator->first);
				enemy->setSetting("entry_y", entry_iterator->second);
				enemy->setSetting("way_point_number", 0);
				result.push_back(enemy);
			}
		}

		mLastCreation = mTimeCounter;

		// ��������� ��������� ����� ��������
		mRandomTime =
			// �� 1 �� 1.5 ������
			(1.f + float(rand())/RAND_MAX/2) * 1000;
	}

	// ������ ���������
	// ���� ���� ���������� ����� �����������
	// ��� � ������� ��� ���� ����
	return result;
}

