#ifndef LOGICNODE_H
#define LOGICNODE_H

#include <string>
#include <memory>

class CSession;

class LogicNode {
    friend class LogicSystem;
public:
    LogicNode(short id, const std::string &body, const std::shared_ptr<CSession> &session);
    LogicNode();
    void setId(short id);

    short getId() const { return _id; }
    const std::string& getBody() const { return _body; }
    std::shared_ptr<CSession> getSession() const { return _session.lock(); }

private:
    std::weak_ptr<CSession> _session;
    std::string _body;
    short _id;
};

#endif //LOGICNODE_H