#include "res.h"

namespace res
{
    Resources ui;

    void load()
    {
        ui.loadXML("res.xml");
    }

    void free()
    {
        ui.free();

    }
}