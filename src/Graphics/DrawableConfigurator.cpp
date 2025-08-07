#include "OpenCore/OpenCore.hpp"

DrawableConfigurator& DrawableConfigurator::Scale(int w, int h)
{
    obj_p->setScale(w,h);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Anchor(AnchorPoint anchor)
{
    obj_p->setAnchor(anchor);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Posite(int x, int y )
{
    obj_p->setPosition(x,y);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Sequence(bool sequential)
{
    obj_p->setSequential(sequential);
    return *this;
}