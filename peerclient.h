#ifndef PEERCLIENT_H
#define PEERCLIENT_H

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "message.h"
#include "stdint.h"
#include <errno.h>

class PeerClient{
    private:
    int client_socket;
    std::string ip;
    int last_message_id = 0;

    public:
    PeerClient(std::string ip, int socket):ip(ip),client_socket(socket){};
    ~PeerClient();

    inline int getSocket(){return this->client_socket;};
    inline std::string getIp(){return this->ip;};
    inline uint32_t getIpInt(){return inet_addr(this->ip.c_str());};

    void send_message(struct MessageToSend msg);
    //Polls the receive buffer but non blocking.
    void receive_message(char* username_buffer,char* recv_buffer);

    void debugPrint();
};

#endif