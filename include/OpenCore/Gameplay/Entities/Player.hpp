#pragma once

// Player.hpp
// 玩家的实体对象

#include "Entity.hpp"

class PlayerEntity : public Entity
{
  public:
    static PlayerEntity &getInstance()
    {
        static PlayerEntity instance;
        return instance;
    }

  private:
};