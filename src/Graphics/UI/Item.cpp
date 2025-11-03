#include"../../Graphics/UI/Item.hpp"
void Item::setNormalTexID(short texID){
    this->texID[0]=texID;
}
void Item::setAnimation(std::vector <short> &texID,short musID){
    short startIndex = this->texID.size();
    this->texID.insert(this->texID.end(), texID.begin(), texID.end()); 
    this->posofAnimation.push_back(startIndex); 
    this->musID.push_back(musID); 
}
short Item::getNormalTexID(){
    return texID[0];
}
std::vector<short> Item::getAniTexID(short pos){
    std::vector<short> result;
    
    // 检查pos是否在合法范围内
    if (pos < 0 || pos >= posofAnimation.size()) {
        return result; // 返回空向量
    }
    
    // 获取起始索引
    size_t startIndex = posofAnimation[pos];
    
    // 获取结束索引
    size_t endIndex;
    if (pos + 1 < posofAnimation.size()) {
        endIndex = posofAnimation[pos + 1]; // 使用下一个动画的起始位置作为结束
    } else {
        endIndex = texID.size(); // 使用texID的末尾作为结束
    }
    
    // 检查索引的合法性
    if (startIndex >= texID.size() || endIndex > texID.size() || startIndex >= endIndex) {
        return result; // 返回空向量
    }
    
    // 使用迭代器范围构造子向量
    result = std::vector<short>(texID.begin() + startIndex, texID.begin() + endIndex);
    
    return result;    
}
short Item::getAniMusID(short pos){
    return musID[pos];
}
short Item::getID(){
    return id;
}
