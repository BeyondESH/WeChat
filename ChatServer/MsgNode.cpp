//
// Created by Beyond on 2025/3/1.
//

#include "MsgNode.h"

MsgNode::MsgNode():currentLen(0),totalLen(0),data(nullptr) {
}

MsgNode::MsgNode(short currentLen, short totalLen, char *data) : currentLen(currentLen), totalLen(totalLen), data(data) {
}

MsgNode::MsgNode(const MsgNode &msgNode):currentLen(msgNode.currentLen),totalLen(msgNode.totalLen),data(msgNode.data) {
}
