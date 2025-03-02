//
// Created by Beyond on 2025/3/2.
//

#ifndef LOGICNODE_H
#define LOGICNODE_H

#include "MsgNode.h"
#include "CSession.h"
#include "LogicSystem.h"
class LogicNode {
    friend class LogicSystem;
public:
    LogicNode(short id,const std::shared_ptr<MsgNode> _msgNode,const std::shared_ptr<CSession> &session);
    LogicNode();
    void setId(short id);
private:
    std::shared_ptr<CSession> _session;
    std::shared_ptr<MsgNode> _msgNode;
    short _id;
};



#endif //LOGICNODE_H
