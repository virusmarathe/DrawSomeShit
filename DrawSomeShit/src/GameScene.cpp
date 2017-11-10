#include "GameScene.h"
#include "MainMenuScene.h"

spGameScene GameScene::instance;

GameScene::GameScene()
{
    //create game actor
    _game = new Game;
    _game->init();
    _game->attachTo(_view);

    //Game Actor would have own Clock.
    //clock is internal time of each Actor
    //by default own clock has only Stage
    //clock could be paused and all children of this Actor would be paused to
    _game->setClock(new Clock);
}

void GameScene::onEvent(Event* ev)
{
    if (ev->type == GameScene::HiddenEvent::EVENT)
    {
        // hide scene here?
    }
}