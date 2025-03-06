//
// Created by Beyond on 2025/3/1.
//

#ifndef MSGNODE_H
#define MSGNODE_H



class MsgNode {
public:
    void clear();
    MsgNode();
    MsgNode(short currentLen,short totalLen,char* data);
    MsgNode(const MsgNode& msgNode);
    short currentLen;
    short totalLen;
    char* data;
};



#endif //MSGNODE_H
