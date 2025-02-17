#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

struct Message{
    char username[64];
    std::string message;
};

struct MessageToSend{
    char* buf;
    int size;
};

#endif