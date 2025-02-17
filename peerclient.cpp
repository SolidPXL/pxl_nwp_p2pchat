#include "peerclient.h"
#include <iostream>

// PeerClient::PeerClient(std::string ip, int socket){
//     this->ip = ip;
//     this->client_socket = socket;
// }

PeerClient::~PeerClient(){

}

void PeerClient::debugPrint(){
    std::cout << "Client=====\n";
    std::cout << "client_socket: "  << this->client_socket << "\n";
    std::cout << "ip: "  << this->ip << "\n";
}

void PeerClient::send_message(struct MessageToSend msg){
    send(this->client_socket,msg.buf,msg.size,0);
}