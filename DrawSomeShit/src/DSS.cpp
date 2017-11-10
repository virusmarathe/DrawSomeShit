#include "oxygine-framework.h"
#include "res.h"
#include "Game.h"
#include "MainMenuScene.h"
#include "GameScene.h"

#include <functional>
using namespace oxygine;

void DSS_preinit() {}

//called from main.cpp
void DSS_init()
{
	res::load();

	GameScene::instance = new GameScene;
	MainMenuScene::instance	 = new MainMenuScene;

	// show main menu
	MainMenuScene::instance->show();
}


//called each frame from main.cpp
void DSS_update()
{

}

//called each frame from main.cpp
void DSS_destroy()
{
	MainMenuScene::instance = 0;
	GameScene::instance = 0;

	res::free();
}
