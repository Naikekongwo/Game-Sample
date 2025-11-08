#include "OpenCore/OpenCore.hpp"

DrawableConfigurator& DrawableConfigurator::Scale(uint16_t w, uint16_t h)
{
    obj_p->setScale(w,h);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Anchor(AnchorPoint anchor)
{
    obj_p->setAnchor(anchor);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Posite(int16_t x, int16_t y )
{
    obj_p->setPosition(x,y);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Sequence(bool sequential)
{
    obj_p->setSequential(sequential);
    return *this;
}