#include "OpenCore/OpenCore.hpp"
#include "Eclipsea/Eclipsea.hpp"

bool MultiImageBoard::onDestroy()
{
    auto result = UIElement::onDestroy();
    // 先执行父类的方法
    Layers.clear();

    return result;
}

void MultiImageBoard::onUpdate(float totalTime)
{
    if(async == true)
    {
        for( auto& entry : Layers)
        {
            // 遍历vector
            entry.get()->onUpdate(totalTime);
        }
    }
}

void MultiImageBoard::onRender()
{
    for(int i = 0; i < Layers.size() ; i++)
    {
        Layers[i].get()->onRender();
    }
}


MultiImageBoard::MultiImageBoard(const std::string& id, uint8_t layer, uint8_t size)
{
    this->id = id;
    this->layer = layer;

    this->directRender = false;

    this->size = static_cast<size_t>(size);

    SDL_Log("MImageBoard::~() created with size: %d", size);
}


bool MultiImageBoard::pushImageBoard(std::vector<std::unique_ptr<Texture>> &list)
{
    if(list.size() != size)
    {
        SDL_Log("MImageBoard::Push... Failed to push texture, it should be %d, but it is a %d", static_cast<int>(size), static_cast<int>(list.size()));
        return false;
    }

    uint8_t iter = 0;

    for( auto it = list.begin(); it != list.end(); )
    {
        auto ImgBoard = std::make_unique<ImageBoard>(std::to_string(iter), layer+iter, std::move(*it));

        ImgBoard->Configure().Parent(this);

        Layers.push_back(std::move(ImgBoard));

        SDL_Log("MImageBoard::Push... successfully pushed a element id %d", iter);
        it = list.erase(it);

        iter++;
    }

    return true;
}

MyAnimationPipeline MultiImageBoard::AnimateAt(uint8_t index)
{
    if(Layers.size() <= index)
    {
        throw std::out_of_range("Index out of range");
    }
    else
    {
        return Layers[index]->Animate();
    }
}

DrawableConfigurator MultiImageBoard::ConfigureAt(uint8_t index)
{
    SDL_Log("CONFIGURE:: Now the size is %d, layers size is %d", static_cast<int>(size), static_cast<int>(Layers.size()));
    if(Layers.size() <= index)
    {
        throw std::out_of_range("Index out of range");
    }
    else
    {
        return Layers[index]->Configure();
    }
}
