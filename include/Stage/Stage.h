#ifndef _STAGE_H_
#define _STAGE_H_

// Stage.h
// 场景的基类

class Stage{
    public:
    Stage() = default;
    virtual ~Stage() = default;


    // 虚函数部分

    // 初始化逻辑
    virtual bool handlEvents() = 0;

    // 更新逻辑
    virtual void onUpdate() = 0;

    // 渲染逻辑
    virtual void onRender() = 0;
};

#endif //_STAGE_H_