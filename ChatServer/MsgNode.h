//
// Created by Beyond on 2025/3/1.
//

#ifndef MSGNODE_H
#define MSGNODE_H

class MsgNode {
public:
    void clear();
    MsgNode(short currentLen,short totalLen);
    MsgNode(const MsgNode& msgNode);
    ~MsgNode();
    short currentLen;
    short totalLen;
    char* data;
};



#endif //MSGNODE_H
