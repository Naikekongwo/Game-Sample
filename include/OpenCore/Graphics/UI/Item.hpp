#pragma once
//Item类的声明
#include<vector>
#include<unordered_map>

class Item
{
    public:
    Item(short id, short texID) : id(id){
        this->texID.push_back(texID) ;
    };
    void setNormalTexID(short texID);
    void setAnimation(std::vector <short> &texID,short musID);
    short getNormalTexID();
    std::vector<short> getAniTexID(short pos);
    short getAniMusID(short pos);
    short getID();


    private:
    
    short id;
    std::vector <short> texID;//一般情况认为0序号为静态展势图像，后续为使用时的动画。
    std::vector <short> musID;//与posofAnimation一一对应，表示动画对应的音乐ID。
    std::vector <short> posofAnimation;//动画相对于texID的起始位置，即textID下标。
};