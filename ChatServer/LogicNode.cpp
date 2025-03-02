//
// Created by Beyond on 2025/3/2.
//

#include "LogicNode.h"

LogicNode::LogicNode(short id, std::shared_ptr<MsgNode> msgNode, const std::shared_ptr<CSession> &session):_id(id),_msgNode(msgNode),_session(session) {
}

LogicNode::LogicNode():_id(-1),_msgNode(nullptr),_session(nullptr) {
}

void LogicNode::setId(short id) {
    this->_id=id;
}
