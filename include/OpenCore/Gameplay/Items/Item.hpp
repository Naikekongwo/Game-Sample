#pragma once

// Item.hpp
// 物品类的基类

#include <cstdint>
#include <memory>
#include <string>

class Texture;

class Item
{
public:

    Item(uint16_t id, const std::string& name, std::unique_ptr<Texture> texture);

    void setID(uint16_t id) { this->id_ = id; }
    uint16_t getID() const { return id_; }

    void setName(const std::string& name) { this->name_ = name; }
    std::string getName() const { return name_; }

    void setMaxSize(uint8_t size) { this->maxStackedSize_  = size; }
    uint16_t getMaxSize() const { return maxStackedSize_; }


    Texture* getTexture() const { return texture_.get(); }
    void setTexture(std::unique_ptr<Texture> texture);

private:
    uint16_t id_;
    std::string name_;
    uint8_t maxStackedSize_;

    std::unique_ptr<Texture> texture_; 
};

using Items = std::pair<std::shared_ptr<Item>, uint8_t>;