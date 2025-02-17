#include "peernetwork.h"
#define _WIN32_WINNT _WIN32_WINNT_WIN7

void OSInit( void )
{
    WSADATA wsaData;
    int WSAError = WSAStartup( MAKEWORD( 2, 0 ), &wsaData ); 
    if( WSAError != 0 )
    {
        fprintf( stderr, "WSAStartup errno = %d\n", WSAError );
        exit( -1 );
    }
}
void OSCleanup( void )
{
    WSACleanup();
}

int main(){
    //////////////////
	//Initialization//
	//////////////////

	OSInit();

    /////////////
	//Execution//
	/////////////

    char ip[16];
    char port[6];
    char username[64];
    std::cout << "Input IP to connect to:\n\r";
    std::cin >> ip;
    std::cout << "Input port of server:\n\r";
    std::cin >> port;
    std::cout << "Input username:\n\r";
    std::cin >> username;

    std::cout << "Setting up network\n\r";
    PeerNetwork network = PeerNetwork(ip,port,username);

    network.start_listening();

    while(1){
        //network.listClients();
        //network.debugPrint();
        // Sleep(10000);
        std::string msg;
        std::getline(std::cin,msg);
        network.broadcast_message(msg);
        
    }

    ////////////
	//Clean up//
	////////////


	OSCleanup();

	return 0;


}