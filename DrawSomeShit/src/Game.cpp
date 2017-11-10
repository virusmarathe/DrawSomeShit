#include "Game.h"
#include "res.h"

Game::Game()
{

}

Game::~Game()
{

}

void Game::init()
{
    //scene layer would have size of display
    setSize(getStage()->getSize());

}

void Game::doUpdate(const UpdateState& us)
{
    
}