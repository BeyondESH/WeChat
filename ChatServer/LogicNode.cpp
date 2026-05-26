#include "LogicNode.h"

LogicNode::LogicNode(short id, const std::string &body, const std::shared_ptr<CSession> &session)
    : _id(id), _body(body), _session(session) {
}

LogicNode::LogicNode() : _id(-1), _body("") {
}

void LogicNode::setId(short id) {
    this->_id = id;
}