// суд€ по запис€м на форуме Marmalade SDK
// Template Unit Test Framework - один, что
// может хорошо работать с Marmalade SDK
#include <tut.hpp>
// + этот framework на чистом —++ (без макросов)
// + позвол€ет переопредел€ть вывод
// + не требует сборки (подключаешь .hpp и всЄ)

#include "FieldUnit.h"

struct shot_data {
	GameObject * shot;
	shot_data() { shot = new TowerDefense::Shot; }
	~shot_data() { delete shot; }
};

typedef tut::test_group<shot_data> gameobject_test_group_type;
gameobject_test_group_type gameobject_test_group("gameobject_tests");

template<>
template<>
void gameobject_test_group_type::object::test<1>() {
	ensure_equals("shot.isset.x is false", shot->getSetting("x").isSet(), false);
	ensure_equals("shot.isset.y is false", shot->getSetting("y").isSet(), false);
}

template<>
template<>
void gameobject_test_group_type::object::test<2>() {

	shot->setSetting("x", 1);
	shot->setSetting("y", 2);

	ensure_equals("shot.isset.x is true", shot->getSetting("x").isSet(), true);
	ensure_equals("shot.isset.y is true", shot->getSetting("y").isSet(), true);

	ensure_equals("shot.setting.x", shot->getSetting("x").getValue(), 1);
	ensure_equals("shot.setting.y", shot->getSetting("y").getValue(), 2);
}

struct battlefield_data {
	BattleField battlefield;
};

typedef tut::test_group<battlefield_data> battlefield_test_group_type;
battlefield_test_group_type battlefield_test_group("battlefield_tests");

template<>
template<>
void battlefield_test_group_type::object::test<1>() {
	ensure_equals("BattleField is empty after construction", battlefield.getBattleMap().size(), 0);
}

#include "LayerConstants.h"

template<>
template<>
void battlefield_test_group_type::object::test<2>() {
	
	battlefield.addShot(new TowerDefense::Shot);
	ensure_equals("BattleField after adding one shot has one shot", battlefield.getShots().size(), 1);

	TowerDefense::Shot * shot;
	battlefield.addShot(shot = new TowerDefense::Shot);
	battlefield.delShot(shot);
	ensure_equals("BattleField delShot deletes one unit", battlefield.getShots().size(), 1);
}

/*
template<>
template<>
void battlefield_test_group_type::object::test<3>() {

	TowerDefense::Shot * shot = new TowerDefense::Shot;
	TowerDefense::Enemy * enemy = new TowerDefense::Enemy(1, 1, 1, 0);
	battlefield.addConnection(shot, enemy);
	battlefield.delConnection(shot);
	ensure_not("BattleField deletes connections by first parameter", battlefield.getConnectedUnit(shot));

	battlefield.addConnection(shot, enemy);
	battlefield.delConnection(enemy);
	ensure_not("BattleField deletes connections by second parameter", battlefield.getConnectedUnit(shot));
}
*/

#include "GameControls.h"

struct gamecontrols_data {
	GameControls game_controls;
	GameControl *game_control;
	gamecontrols_data() { game_controls.addControl(game_control = new TowerDefense::PlayButton); }
	~gamecontrols_data() { }
};

typedef tut::test_group<gamecontrols_data> gamecontrols_test_group_type;
gamecontrols_test_group_type gamecontrols_test_group("gamecontrols_tests");

template<>
template<>
void gamecontrols_test_group_type::object::test<1>() {

	ensure_equals("testing if game control is disabled by default", game_control->isEnabled(), false);
	
	game_control->enable();
	ensure_equals("testing if game control enables", game_control->isEnabled(), true);
	
	game_control->disable();
	ensure_equals("testing if game control disables", game_control->isEnabled(), false);
}

template<>
template<>
void gamecontrols_test_group_type::object::test<2>() {
	
	ensure_equals("testing whether control added", game_controls.getControls().size(), 1);
	ensure_not("testing getting control by name", !game_controls.getControlByName(game_control->getObjectName()));
	
	if (game_controls.getControlByName(game_control->getObjectName())->isClickable())
		ensure_equals("testing getting control by clickable property", game_controls.getClickableControls().size(), 1);
}

/*
#include "Iw2D.h"
#include "IwResManager.h"

struct drawhelper_data {
	DrawHelper draw_helper;
	drawhelper_data() {
		//Iw2DInit();
		//IwResManagerInit();
		draw_helper.construct(20, 11);
	}

	~drawhelper_data() {
		draw_helper.destruct();
		//IwResManagerTerminate();
		//Iw2DTerminate();
	}
};

typedef tut::test_group<drawhelper_data> drawhelper_test_group_type;
drawhelper_test_group_type drawhelper_test_group("drawhelper_tests");

template<>
template<>
void drawhelper_test_group_type::object::test<1>() {
	ensure_equals("test scalemetrics interface center x", draw_helper.getCellCenterX(), 10);
	ensure_equals("test scalemetrics interface center y", draw_helper.getCellCenterY(), 5.5);

	//ensure_equals("test coordinate scaling x screen size", draw_helper.getScreenX(20), Iw2DGetSurfaceWidth());
	//ensure_equals("test coordinate scaling y screen size", draw_helper.getScreenY(11), Iw2DGetSurfaceHeight());

	ensure_equals("test coordinate scaling x zero", draw_helper.getScreenX(0), 0);
	ensure_equals("test coordinate scaling y zero", draw_helper.getScreenY(0), 0);

	//ensure_equals("test coordinate scaling x center size", draw_helper.getScreenX(10), Iw2DGetSurfaceWidth() / 2);
	//ensure_equals("test coordinate scaling y center size", draw_helper.getScreenY(5.5), Iw2DGetSurfaceHeight() / 2);

	//ensure_equals("test cell -> screen center x scaling", draw_helper.getCellX(Iw2DGetSurfaceWidth() / 2.), 10.f);
	//ensure_equals("test cell -> screen center y scaling", draw_helper.getCellY(Iw2DGetSurfaceHeight() / 2.f), 5.5f);
}
*/

#include "WorldCreator.h"

struct worldcreator_data {
	WorldCreator world_creator;
	BattleField  battlefield;
	worldcreator_data() : world_creator(battlefield) { world_creator.create(); }
};

typedef tut::test_group<worldcreator_data> worldcreator_test_group_type;
worldcreator_test_group_type worldcreator_test_group("worldcreator_tests");

template<>
template<>
void worldcreator_test_group_type::object::test<1>() {
	WorldCreator::PointsList entries = world_creator.getMapEntries();
	ensure_equals("test entries number", entries.size(), 2);
	for (WorldCreator::PointsList::const_iterator entry_iterator = entries.begin();
		entry_iterator != entries.end();
		++entry_iterator) {
			ensure_equals("test if waypoint 0 x is entry for given entry", 
				world_creator.getWayPoint(*entry_iterator, 0).first, entry_iterator->first
			);
			ensure_equals("test if waypoint 0 y is entry for given entry", 
				world_creator.getWayPoint(*entry_iterator, 0).second, entry_iterator->second
			);

	}
}

#include "WorldProcessor.h"
struct worldprocessor_data {
	WorldCreator world_creator;
	BattleField  battlefield;
	WorldProcessor * world_processor;
	TowerDefense::Tower * tower;
	TowerDefense::Enemy * enemy;

	worldprocessor_data() : world_creator(battlefield), world_processor(0) { 
		world_creator.create(); 
		battlefield.addTower(tower = world_creator.createTower(WorldCreator::TOWERTYPE::TOWER_1));
		tower->setSetting("x", 17.f);
		tower->setSetting("y",  2.f);
		battlefield.addEnemy(enemy = world_creator.createEnemy(WorldCreator::ENEMYTYPE::ENEMY_1));
		enemy->setSetting("x", 17.f);
		enemy->setSetting("y",  3.f);
		world_processor = new WorldProcessor(world_creator, battlefield);
	}
	~worldprocessor_data() {
		delete world_processor;
	}
};

typedef tut::test_group<worldprocessor_data> worldprocessor_test_group_type;
worldprocessor_test_group_type worldprocessor_test_group("worldprocessor_tests");

template<>
template<>
void worldprocessor_test_group_type::object::test<1>() {
	world_processor->mTimeCounter = 1000;
	world_processor->processTowers(100);
	ensure_equals("shot created by processTowers", battlefield.getShots().size() == 1, true);
	enemy->setSetting("current_HP", 1.f);
	for (int time = 0; time < gShotLife + 100; time+=100) 
		world_processor->processShots(100);
	ensure_equals("shot killed the enemy", battlefield.getEnemies().size() == 0, true);
	ensure_equals("shot destroyed", battlefield.getShots().size() == 0, true);
}

template<>
template<>
void worldprocessor_test_group_type::object::test<2>() {
	world_processor->initStatistics();
	enemy->setSetting("way_point_number", 3.f);
	enemy->setSetting("entry_x", 20.f);
	enemy->setSetting("entry_y",  3.f);
	float speed = enemy->getSetting("speed").getValue();
	float type = enemy->getSetting("type").getValue();
	for (int i = 0; i < int(speed * 18 /* cells */ * 10 /* deltas in second */) ; ++i) 
		world_processor->processEnemies(100);
	ensure_equals("alive tank destroyed", battlefield.getEnemies().size(), 0);
	ensure_equals("alive statistics updated", world_processor->mAliveStatistics[type], 1);
}

#include "GameMode.h"

struct preparingfieldmode_data {
	TowerDefense::PreparingFieldMode * preparingfield_mode;
	WorldCreator world_creator;
	BattleField  battlefield;
	WorldProcessor * world_processor;
	TowerDefense::Tower * tower;
	TowerDefense::TowerSlot * slot;
	GameControls game_controls;

	preparingfieldmode_data() : world_creator(battlefield), world_processor(0) {
		world_creator.create(); 
		battlefield.addTower(tower = world_creator.createTower(WorldCreator::TOWERTYPE::TOWER_1));
		tower->setSetting("x", 17.f);
		tower->setSetting("y",  2.f);
		battlefield.addTowerSlot(slot = new TowerDefense::TowerSlot(10.f, 2.f));
		world_processor = new WorldProcessor(world_creator, battlefield);

		DrawHelper::construct(world_creator.getCellsX(), world_creator.getCellsY());

		preparingfield_mode = new TowerDefense::PreparingFieldMode(battlefield, game_controls, world_creator, *world_processor);
		preparingfield_mode->initialize();
	}

	~preparingfieldmode_data() {
		delete world_processor;
		delete preparingfield_mode;

		DrawHelper::destruct();
	}
};

typedef tut::test_group<preparingfieldmode_data> preparingfield_test_group_type;
preparingfield_test_group_type preparingfield_test_group("preparingfield_tests");

template<>
template<>
void preparingfield_test_group_type::object::test<1>() {
	ensure_equals("test we have 16 + 1 tower slots", battlefield.getTowerSlots().size(), 17);
	ensure_equals("test we have one tower", battlefield.getTowers().size(), 1);
	preparingfield_mode->processUserEvent(DrawHelper::getScreenX(10.1f), DrawHelper::getScreenY(2.1f));
	ensure_equals("test if menu tower choice menu appeared", preparingfield_mode->mTowerChoiceMenu->isEnabled(), true);
	preparingfield_mode->processUserEvent(DrawHelper::getScreenX(10.1f), DrawHelper::getScreenY(5.6f));
	ensure_equals("test if menu tower choice menu disabled", preparingfield_mode->mTowerChoiceMenu->isEnabled(), false);
	ensure_equals("test if tower slot became tower", battlefield.getTowers().size(), 2);
	ensure_equals("test we have one less tower slots", battlefield.getTowerSlots().size(), 16);
}


tut::test_runner_singleton runner;

//#include <s3e.h>
#include <sstream>
#include <fstream>
#include <iostream>

class callback : public tut::callback {

public:
 
  void run_started() {
	  //s3eDebugOutputString("=== Starting tests ==="); 
	  std::cout << "=== Starting tests ===" << std::endl;
  }
  
  void test_completed(const tut::test_result& result) {
	std::stringstream text;
	text << result.group << ": " << result.test << ": " << (result.message.empty() ? "passed" : result.message) << std::flush;
	//s3eDebugOutputString(text.str().c_str());
	std::cout << text.str() << std::endl;
  }

  void run_completed() { 
	  //s3eDebugOutputString("=== Tests finished ==="); 
	  std::cout << "=== Tests finished ===" << std::endl;
  }
};

int main() {
	callback debug_console_writer;
	runner.get().set_callback(&debug_console_writer);
	runner.get().run_tests();
	std::string dummy;
	std::cin >> dummy;
	return 0;
}
