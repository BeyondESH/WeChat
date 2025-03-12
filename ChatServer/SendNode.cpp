//
// Created by Beyond on 2025/3/8.
//

#include "SendNode.h"

SendNode::SendNode(char *data, short id, short total_len):_data(data),_id(id),_totalLen(total_len){
}

SendNode::~SendNode() {
    if (_data!=nullptr) {
        delete[] _data;
        _data=nullptr;
    }
}
