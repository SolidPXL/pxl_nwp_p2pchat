#ifndef PEERNETWORK_H
#define PEERNETWORK_H

#include "peerclient.h"
#include "chat.h"
#include <vector>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>
#include <stdio.h> //for fprintf, perror
#include <stdlib.h> //for exit
#include <string.h> //for memset
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <cstdio>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

class PeerNetwork{

    private:
    std::string username;
    char port[8];


    std::vector<PeerClient> clients;

    //server socket

    std::mutex connectionMutex;  
    std::atomic<bool> running;  
    std::thread listenerThread;
    std::thread clientThread;
    int listenSocket;

    void listen_for_connections();

    public:
    PeerNetwork(char* port, char* username);
    ~PeerNetwork();

    void join_network(char* ip, char* port);
    void remove_client_from_network(int socket);

    Chat Chat;
    
    void start_listening();
    void stop_listening();

    //Debugging
    void debugPrint();
    void listClients();

    //Function used to check for closed tcp connections. Call every time before sending a chat message to prevent sending to closed connections
    void poll_clients();
    void broadcast_message(std::string_view message);

    std::string get_client_ip_list();
    //Return size in bytes of the filled data
    int get_client_ip_list_int(uint32_t* buf, int size);


};

#endif