//
// Created by Beyond on 2025/3/1.
//

#include "MsgNode.h"

#include <cstring>

void MsgNode::clear() {
    if (totalLen!=0) {
        memset(data,0,totalLen);
    }
    currentLen=0;
}

MsgNode::MsgNode(short currentLen, short totalLen) : currentLen(currentLen), totalLen(totalLen){
    if (totalLen>0) {
        this->data=new char[totalLen];
        memset(this->data,0,totalLen);
    }else {
        this->data=nullptr;
    }
}

MsgNode::MsgNode(const MsgNode &msgNode):currentLen(msgNode.currentLen),totalLen(msgNode.totalLen),data(msgNode.data) {
    if (msgNode.data != nullptr && totalLen > 0) {
        data = new char[totalLen];
        memcpy(data, msgNode.data, totalLen);
    } else {
        data = nullptr;
    }
}

MsgNode::~MsgNode() {
    if (data!=nullptr) {
        delete[] data;
        data=nullptr;
    }
}
