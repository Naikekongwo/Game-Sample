#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

DrawableConfigurator& DrawableConfigurator::Scale(float w, float h)
{
    obj_p->setScale(w,h);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Anchor(AnchorPoint anchor)
{
    obj_p->setAnchor(anchor);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Posite(float xPercent, float yPercent)
{
    obj_p->setPosition(xPercent, yPercent);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Sequence(bool sequential)
{
    obj_p->setSequential(sequential);
    return *this;
}

DrawableConfigurator& DrawableConfigurator::Parent(IDrawableObject* parent)
{
    obj_p->setParentContainer(parent);
    return *this;
}