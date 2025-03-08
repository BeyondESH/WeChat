//
// Created by Beyond on 2025/3/8.
//

#ifndef SENDNODE_H
#define SENDNODE_H

class MsgNode;

class SendNode {
public:
    SendNode(char *data, short id, short total_len);
    char *data;
    short id;
    short totalLen;
};



#endif //SENDNODE_H
