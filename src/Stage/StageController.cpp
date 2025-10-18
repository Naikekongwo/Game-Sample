// StageController.cpp
// 场景控制器的具体实现
// 以及场景类的部分实现

#include "OpenCore/OpenCore.hpp"

short StageController::changeStage(std::unique_ptr<Stage> newStage)
{   
    if(currentStage!=nullptr){
        stageCache_[currentId]=std::move(currentStage);
    }
    
    short newId=1;
    while(newId==0||newId==-1||stageCache_.find(newId)!=stageCache_.end()){
        newId++;
        if(newId==-1){//表示循环一周已满
            SDL_Log("The stageCache_ has been full.");
            return -1;//表示放入失败
        }
    }
    currentStage = std::move(newStage);
    currentId=newId;
    return currentId;//告知id
}

bool StageController::changeStage(short id){
    if(stageCache_.find(id)!=stageCache_.end()){
        SDL_Log("The id was not existing.");
        return false;
    }
    if(currentStage!=nullptr){
        stageCache_[currentId]=std::move(currentStage);
    }
    currentId=id;
    currentStage=std::move(stageCache_[id]);
    return true;
}

short StageController::addStageToCache(std::unique_ptr<Stage> newStage,short id){
    if(stageCache_.find(id)!=stageCache_.end()||id==0||id==-1||id==currentId){
        SDL_Log("The id is invalid");//id不合法
        short newId=1;
        while(newId==0||newId==-1||stageCache_.find(newId)!=stageCache_.end()||newId==currentId){
            newId++;
            if(newId==-1){//表示循环一周已满
                SDL_Log("The stageCache_ has been full.");
                return -1;//表示放入失败
            }
        }
        stageCache_[newId]=std::move(newStage);
        SDL_Log("the new id is %d.",newId);
        return newId;//告知新id
    }else{
        stageCache_[id]=std::move(newStage);
        return 0;//表示放入顺利
    }
    
}

bool StageController::deleteStage(short id){
    if(stageCache_.find(id)==stageCache_.end()){
        SDL_Log("The id is not existing.");
        return false;
    }
    stageCache_[id].reset();
    stageCache_.erase(id);
    return true;
}

bool StageController::handlEvents(SDL_Event *event)
{
    return (currentStage) ? currentStage->handlEvents(event) : false;
}

void StageController::onUpdate()
{
    if (currentStage)
        currentStage->onUpdate();
}

void StageController::onRender()
{
    if (currentStage)
        currentStage->onRender();
}