#include "peernetwork.h"
#define _WIN32_WINNT _WIN32_WINNT_WIN7


int main() {


    /////////////
    //Execution//
    /////////////

    char ip[16];
    char port[6];
    char username[64];
    char selection = '\0';
    std::cout << "Want to start as server or connect to network?\n";
    while (!(selection == '1' || selection == '2')) {
        std::cout << "(1) Start as server (2) Start as client\n";
        std::cin >> selection;
    }

    std::cout << "Input port of server:\n";
    std::cin >> port;
    std::cout << "Input username:\n";
    std::cin >> username;

    std::cout << "Setting up network\n";
    PeerNetwork network = PeerNetwork(port, username);

    if (selection == '2') {
        std::cout << "Input IP of network to join:\n";
        std::cin >> ip;
        // Make initial connection
        network.join_network(ip, port);
    }

    network.start_listening();

    while (true) {
        std::string msg;
        std::getline(std::cin, msg);
        network.broadcast_message(msg);
        sleep(10); // Linux sleep (in seconds)
    }


    return 0;
}