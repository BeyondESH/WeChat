//
// Created by Beyond on 2025/3/1.
//

#include "MsgNode.h"

#include <cstring>

void MsgNode::clear() {
    memset(data,0,totalLen);
    currentLen=0;
}

MsgNode::MsgNode():currentLen(0),totalLen(0),data(nullptr) {
}

MsgNode::MsgNode(short currentLen, short totalLen, char *data) : currentLen(currentLen), totalLen(totalLen), data(data) {
}

MsgNode::MsgNode(const MsgNode &msgNode):currentLen(msgNode.currentLen),totalLen(msgNode.totalLen),data(msgNode.data) {
}
