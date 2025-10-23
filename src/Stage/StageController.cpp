// StageController.cpp
// 场景控制器的具体实现
// 以及场景类的部分实现

#include "OpenCore/OpenCore.hpp"
#include <algorithm> // std::swap

void StageController::changeStage(std::unique_ptr<Stage> newStage)
{   
    // 如果 currentStages 为空，则先加入三个空的智能指针
    if (currentStages.empty()) {
        currentStages.resize(3); // 创建三个空指针
    }

    // 将 newStage 的所有权移交给 vector[0]
    // 如果原来位置有对象，它会被自动释放
    currentStages[0] = std::move(newStage);
}
void StageController::changeStage(std::unique_ptr<Stage> newStage,int index){
    if (currentStages.empty()) {
        currentStages.resize(3); 
    }
    currentStages[index] = std::move(newStage);
}
void StageController::addTempStage(std::unique_ptr<Stage> newStage){
    if (currentStages.empty()) {
        currentStages.resize(3); 
    }
    currentStages.push_back(std::move(newStage));
}
bool StageController::deleteStage(int index){
     if (index != 0 && index != 1 && index != 2) {
        return false;
    }
    if (currentStages.size() <= index) {
        return false; 
    } 
    // 释放指定位置的智能指针，但保留位置（设置为空指针）
    currentStages[index].reset();
    return true; // 操作成功
}
bool StageController::deleteTempStage(){
     if (currentStages.size() <= 3) {
        return false;
    }
    if (currentStages.back()) {
        currentStages.back().reset(); 
    }
    currentStages.pop_back();
    return true;
}
void StageController::swapStage(int index1,int index2){
    if (index1 < 0 || index1 > 2 || index2 < 0 || index2 > 2 || index1 == index2) {
        return;
    }
    if (index1 >= currentStages.size() || index2 >= currentStages.size()) {
        return;
    }    
    std::swap(currentStages[index1], currentStages[index2]);
}



bool StageController::handlEvents(SDL_Event *event)//这里基类的handleEvents需要修改不能返回空值而是bool值
{
    if (currentStages.empty()) 
        return false;
    
    // 依次寻找，找到第一个处理事件的 stage 后立即退出
    for (auto& stage : currentStages) {
        if (stage && stage->handlEvents(event)) {
            return true; // 找到后立即退出
        }
    }
    return false;
}

void StageController::onUpdate()
{
    if (currentStages.empty())
        return;
    
    // 所有非空 stage 都执行更新
    for (auto& stage : currentStages) {
        if (stage) {
            stage->onUpdate();
        }
    }
}

void StageController::onRender()
{
    if (currentStages.empty())
        return;
    
    for (auto& stage : currentStages) {
        if (stage) {
            stage->onRender();
        }
    }
}