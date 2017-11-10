#include "oxygine-framework.h"
#include <functional>
using namespace oxygine;

//it is our resources
//in real project you would have more than one Resources declarations.
//It is important on mobile devices with limited memory and you would load/unload them
Resources gameResources;

void DSS_preinit() {}

//called from main.cpp
void DSS_init()
{
    //load xml file with resources definition
    gameResources.loadXML("res.xml");

}


//called each frame from main.cpp
void DSS_update()
{

}

//called each frame from main.cpp
void DSS_destroy()
{
    //free previously loaded resources
    gameResources.free();
}
