#include "chat.h"

struct Message Chat::processMessage(char* buf, int limit){
    int id = static_cast<int>(buf[0]);
    char* name = buf+1;
    char* message;

    for(int i=1; i<limit; i++){
        if(buf[i]=='\0'){
            message = buf+i+1;
            break;
        }
    }
    buf[limit-1] = '\0';

    Message msg;
    memcpy(msg.username, name, 64);
    msg.message = message;

    return msg;
}

struct MessageToSend Chat::prepareMessage(struct Message message){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    char id = static_cast<char>(dist(gen));

    char msgbuf[1024];
    int length = 1;

    msgbuf[0] = id;
    memcpy(msgbuf+1, message.username, 64);

    for(int i=1; i<sizeof(msgbuf); i++){
        if(msgbuf[i]=='\0'){
            memcpy(msgbuf+i+1, message.message.c_str(), 1024-i);
            length += i;
            break;
        }
    }

    length += message.message.length();

    struct MessageToSend msg = {
        msgbuf,
        length
    };

    return msg;
}

void Chat::addMessage(struct Message message){
    this->chatlog.push_back(message);

    //Push it to cout
    std::cout << "\033[31m" << message.username << "\033[0m: " << message.message << "\n";
}
