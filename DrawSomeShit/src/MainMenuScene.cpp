#include "MainMenuScene.h"
#include "Sprite.h"
#include "initActor.h"
#include "res.h"
#include "UIButton.h"
#include "GameScene.h"

spMainMenuScene MainMenuScene::instance;

MainMenuScene::MainMenuScene() : Scene()
{
    //create background
    spSprite bg = new Sprite;
    bg->setResAnim(res::ui.getResAnim("mainmenubg"));
    bg->attachTo(_view);

    //create play button at center
    spUIButton btn = initActor(new UIButton,
                             arg_name = "play",
                             arg_resAnim = res::ui.getResAnim("button"),
                             arg_anchor = Vector2(0.5f, 0.5f),
                             arg_pos = _view->getSize() / 2,
                             arg_attachTo = _view);
	
	btn->setText("Draw!");
    //handle click to button
    btn->addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainMenuScene::onEvent));


    //create menu button
    btn = initActor(new UIButton,
                    arg_name = "exit",
                    arg_resAnim = res::ui.getResAnim("close"),
                    arg_anchor = Vector2(0.5f, 0.5f),
                    arg_attachTo = _view);

    //align it to top right
    btn->setX(_view->getWidth() - btn->getWidth() / 2);
    btn->setY(btn->getHeight() / 2);

    btn->addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainMenuScene::onEvent));
}

void MainMenuScene::onEvent(Event* ev)
{
    string id = ev->currentTarget->getName();
    if (id == "exit")
    {
        //request quit
        core::requestQuit();
    }

    if (id == "play")
    {
        //clicked to play button
        //change scene
        changeScene(GameScene::instance);
    }
}