//
// Created by Beyond on 2025/3/8.
//

#ifndef SENDNODE_H
#define SENDNODE_H

class MsgNode;

class SendNode {
public:
    SendNode(char *data, short id, short total_len);
    ~SendNode();
    char *_data;
    short _id;
    short _totalLen;
};



#endif //SENDNODE_H
