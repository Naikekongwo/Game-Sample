/**
 * @file MultiImageBoard.hpp
 * @brief 多层图片容器控件。
 * 
 * 管理多个 ImageBoard 图层，支持按图层顺序统一渲染和更新，
 * 可为每个子图层单独配置动画和视觉属性。
 */

#ifndef _MULTI_IMAGEBOARD_H_
#define _MULTI_IMAGEBOARD_H_

#include "OpenCore/Runtime/Graphics/IDrawableObject/UIElement.hpp"

#include <memory>
#include <string>
#include <vector>

using std::unique_ptr;
using std::vector;

class ImageBoard;  // 前向声明

/**
 * @class MultiImageBoard
 * @brief 多层图片控件，内部维护一组 ImageBoard。
 * 
 * 将多个纹理作为独立图层组合，按添加顺序依次绘制（后添加的在上层）。
 * 支持异步更新（默认开启），可为每个子图层单独配置动画和视觉属性。
 */
class MultiImageBoard : public UIElement
{
public:
    /**
     * @brief 构造多层图片控件。
     * @param id    唯一标识符。
     * @param layer 渲染图层（作为容器的基础层，子图层在此基础上递增）。
     * @param size  预期包含的子图层数量（需与后续 pushImageBoard 的纹理数量匹配）。
     */
    MultiImageBoard(const std::string &id, uint8_t layer, uint8_t size);
    bool onDestroy() override;
    void onUpdate(float totalTime) override;
    void Draw() override;

    /**
     * @brief 推送纹理列表，为每个纹理创建一个 ImageBoard 子图层。
     * @param list 纹理智能指针列表，函数会转移所有权，调用后列表将为空。
     * @return true  若纹理数量与预设 size 匹配且全部成功创建；
     * @return false 若数量不匹配。
     */
    bool pushImageBoard(vector<unique_ptr<Texture>> &list);

    /**
     * @brief 获取指定索引子图层的动画配置器。
     * @param index 子图层索引（0 ~ size-1）。
     * @return AnimationPipeline 对象，用于链式定义动画。
     * @throws std::out_of_range 索引越界时抛出。
     */
    AnimationPipeline AnimateAt(uint8_t index);

    /**
     * @brief 获取指定索引子图层的视觉属性配置器。
     * @param index 子图层索引（0 ~ size-1）。
     * @return DrawableConfigurator 对象，用于链式设置位置、缩放等。
     * @throws std::out_of_range 索引越界时抛出。
     */
    DrawableConfigurator ConfigureAt(uint8_t index);

private:
    bool async = true;                          ///< 是否异步更新子图层（默认开启）
    bool ready = false;                         ///< 是否就绪（当前未使用）
    size_t size = 0;                            ///< 预期子图层数量
    vector<unique_ptr<ImageBoard>> Layers;      ///< 子图层列表
};

#endif //_MULTI_IMAGEBOARD_H_