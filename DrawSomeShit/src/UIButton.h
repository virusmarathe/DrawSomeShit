#pragma once
#include "oxygine-framework.h"
using namespace oxygine;
using namespace std;

DECLARE_SMART(UIButton, spUIButton);
class UIButton: public Sprite
{
public:
    UIButton();

    void setText(const string& txt);

private:
    void onEvent(Event*);

    spTextField _text;
};