#include "chat.h"

struct Message Chat::processMessage(char* buf, int limit){
    int id = (int)(buf[0]);
    char* name = buf+1;
    char* message;

    for(int i=1;i<limit;i++){
        if(buf[i]=='\0'){
            message = buf+i+1;
            break;
        }
    }
    buf[limit-1] = '\0';

    Message msg;
    memcpy(msg.username,name,64);
    msg.message = message;

    return msg;


}

struct MessageToSend Chat::prepareMessage(struct Message message){
    srand(time(NULL));
    char id = rand()%255;

    char msgbuf[1024];
    int length = 1;

    msgbuf[0] = id;
    memcpy(msgbuf+1,message.username,64);

    for(int i=1;i<sizeof(msgbuf);i++){
        if(msgbuf[i]=='\0'){
            memcpy(msgbuf+i+1,message.message.c_str(),1024-i);
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
    std::cout << RED << message.username << RESET << ": " << message.message << "\n";

}