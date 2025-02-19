#include "peernetwork.h"

PeerNetwork::PeerNetwork(char* port,char* username){
    //Setup other variables
    this->username = username;
    strcpy(this->port,port);
    this->running = false;


    //Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof(internet_address_setup) );
	internet_address_setup.ai_family = AF_INET;
	internet_address_setup.ai_socktype = SOCK_STREAM;
	internet_address_setup.ai_flags = AI_PASSIVE;
	int getaddrinfo_return = getaddrinfo( NULL, port, &internet_address_setup, &internet_address_result );
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		std::cout << "Error in getting addr info";
		throw 1;
	}

	int internet_socket = -1;
	struct addrinfo * internet_address_result_iterator = internet_address_result;
	while( internet_address_result_iterator != NULL )
	{
		//Step 1.2
		internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
		if( internet_socket == -1 )
		{
			std::cout << "Error in creating server socket";
			throw 2;
		}
		else
		{
			//Step 1.3
			int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			if( bind_return == -1 )
			{
				std::cout << "Error in binding server";
				closesocket( internet_socket );
                throw 3;
			}
			else
			{
				//Step 1.4
				int listen_return = listen( internet_socket, 1 );
				if( listen_return == -1 )
				{
					std::cout << "Error in setting socket to listen";
					closesocket( internet_socket );
					throw 4;
				}
				else
				{
					break;
				}
			}
		}
		internet_address_result_iterator = internet_address_result_iterator->ai_next;
	}

	freeaddrinfo( internet_address_result );

	if( internet_socket == -1 )
	{
		fprintf( stderr, "socket: no valid socket address found\n" );
		throw 5;
	}

	this->listenSocket = internet_socket;
}

PeerNetwork::~PeerNetwork(){
    this->stop_listening();
    //Clean up clients

}

void PeerNetwork::join_network(char* ip, char* port){
	//Step 1.1
	struct addrinfo internet_address_setup;
	struct addrinfo * internet_address_result;
	memset( &internet_address_setup, 0, sizeof(internet_address_setup) );
	internet_address_setup.ai_family = AF_INET;
	internet_address_setup.ai_socktype = SOCK_STREAM;
	std::cout << "connecting tp ip: "<< ip << " usin port: "<< port <<"\n";
	int getaddrinfo_return = getaddrinfo( ip, port, &internet_address_setup, &internet_address_result );
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		throw 3;
		exit( 1 );
	}

	int internet_socket = -1;
	struct addrinfo * internet_address_result_iterator = internet_address_result;
	while( internet_address_result_iterator != NULL )
	{
		//Step 1.2
		internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
		if( internet_socket == -1 )
		{
			throw 4;
			perror( "socket" );
		}
		else
		{
			//Step 1.3
			int connect_return = connect( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
			if( connect_return == -1 )
			{
				throw 5;
				perror( "connect" );
				closesocket( internet_socket );
			}
			else
			{
				break;
			}
		}
		internet_address_result_iterator = internet_address_result_iterator->ai_next;
	}

	freeaddrinfo( internet_address_result );

	if( internet_socket == -1 )
	{
		fprintf( stderr, "socket: no valid socket address found\n" );
		exit( 2 );
	}

	//Wait to receive their iplist
	std::cout << "Made connection with client on socket: "<< internet_socket << "\n";

	int timeout = 1000;
	setsockopt(internet_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(timeout));
	int number_of_bytes_received = 0;
	uint32_t ipList[256];
	memset(ipList,'\0',sizeof(ipList));
	number_of_bytes_received = recv( internet_socket, (char*)ipList, sizeof(ipList), 0 );

	if( number_of_bytes_received == -1 )
	{
		//Connection timed out, no iplist received

		//TODO add check to see if it is worth to stay connected to the user
	} else if(number_of_bytes_received == 0){
		std::cout << "No ip list received";
		closesocket(internet_socket);
		throw 2;
	}

	int resettime = 1000;
	setsockopt(internet_socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&resettime,sizeof(resettime));

	//Add socket to list of clients if they provide a valid iplist
	std::string ipstring = ip;
	PeerClient client = PeerClient(ipstring,internet_socket);

	this->clients.push_back(client);

	std::cout << "First ip received:" << ipList[0] << "\n";

	//Check if IP is already connected to, if not recursive call
	int i=0;
	while(ipList[i]!=0&&i<=256){
		//convert to string
		struct in_addr addr;
    	//addr.s_addr = htonl(ipList[i]); // Ensure network byte order
		addr.s_addr = ipList[i]; // Ensure network byte order
		std::string convip = inet_ntoa(addr);

		//Check if ip is is client list
		int client_in_list = 0;
		for(int j=0;j<this->clients.size();j++){
			if(this->clients[i].getIp()==convip){
				client_in_list = 1;
				break;
			}
		}
		if(client_in_list == 0){
			try{
				this->join_network((char*)(convip.c_str()),port);
			} catch(int err){
				closesocket(internet_socket);
				throw 3;
			}
			
		}

		i++;
	}


}

void PeerNetwork::remove_client_from_network(int socket){
	//find socket and remove
	this->connectionMutex.lock();
	auto it = this->clients.begin();
	for(int i=0;i<this->clients.size();i++){
		if(this->clients[i].getSocket()==socket){
			closesocket(this->clients[i].getSocket());
			this->clients.erase(it+i);
		}
	}
	this->connectionMutex.unlock();
}

void PeerNetwork::listen_for_connections(){
    while (running) {
        struct sockaddr_storage client_internet_address;
        socklen_t client_internet_address_length = sizeof(client_internet_address);
        int client_socket = accept(this->listenSocket, (sockaddr*)&client_internet_address, &client_internet_address_length);

		Sleep(500);

        if (client_socket <= 0) {
            if (!this->running) break;  // Exit loop if stopping
            //closesocket(this->listenSocket);
            continue;
        }

        std::lock_guard<std::mutex> lock(connectionMutex);

		

        //Get inet string from new connection
		struct sockaddr_in *addr4 = (sockaddr_in*)&client_internet_address;
		char stringip[16];
        inet_ntop(AF_INET, &(addr4->sin_addr),stringip,16);
		std::string ip(stringip);

        PeerClient client = PeerClient(ip,client_socket);

		//Send client list
		uint32_t ipbuffer[254];
		int bytes_to_send = this->get_client_ip_list_int(ipbuffer,254);

		// std::string clientList = this->get_client_ip_list();
		// clientList = clientList + client.getIp();

		//send(client_socket,clientList.c_str(),clientList.length(),0);
		send(client_socket,(char*)ipbuffer,bytes_to_send,0);

        //Add client to list
        this->clients.emplace_back(client);
    }
}

void PeerNetwork::start_listening(){
    this->running = true;
    listenerThread = std::thread(&PeerNetwork::listen_for_connections,this);
	clientThread = std::thread(PeerNetwork::poll_clients,this);
}

void PeerNetwork::stop_listening(){
    this->running = false;
    if(listenerThread.joinable()){
        listenerThread.join();
    }
}

void PeerNetwork::listClients(){
	for(int i = 0; i<this->clients.size();i++){
		this->clients[i].debugPrint();
	}
	

}

void PeerNetwork::debugPrint(){
	std::cout << "username: "  << this->username << "\n";
	std::cout << "port: "  << this->port << "\n";
	std::cout << "connectionMutex: "  << this->connectionMutex.native_handle() << "\n";
	std::cout << "listenerThread: "  << this->listenerThread.get_id() << "\n";
	std::cout << "listenSocket: "  << this->listenSocket << "\n";

}

void PeerNetwork::poll_clients(){
	while(running){
		//Create set
		this->connectionMutex.lock();
		std::vector<int> fdlist;
		fd_set readSet;
		FD_ZERO(&readSet);
		//Loop over each item
		for(int i=0;i<this->clients.size();i++){
			//Check if socket received a message
			fdlist.push_back(this->clients[i].getSocket());
			FD_SET(this->clients[i].getSocket(), &readSet);
		}
		this->connectionMutex.unlock();


		timeval timeout = {0, 100000};  // 100ms timeout

		int result = select(0, &readSet, NULL, NULL, &timeout);

		if (result > 0) {
			for(int i=0;i<fdlist.size();i++){
				if(FD_ISSET(fdlist[i],&readSet)){
					//Socket is readable
					char buffer[1024];
					memset(buffer,'\0',1024);
					int bytesReceived = recv(fdlist[i], buffer, sizeof(buffer), 0);
					if (bytesReceived == 0 || (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)) {
						// Connection closed, remove from the set
						this->remove_client_from_network(fdlist[i]);
						FD_CLR(fdlist[i], &readSet);
					} else {
						// Process received data...
						struct Message message = this->Chat.processMessage(buffer,1024);
						this->Chat.addMessage(message);
					}
				}
			}
		} else if (result == 0) {
			// Timeout occurred
		} else {
			// Error, most likely no clients are connected
		}
	}
	
}

void PeerNetwork::broadcast_message(std::string_view message){
	struct Message readmsg;
	readmsg.message = message;
	strcpy(readmsg.username,this->username.c_str());
	struct MessageToSend msg = this->Chat.prepareMessage(readmsg);

	for(int i=0;i<this->clients.size();i++){
		this->clients[i].send_message(msg);
	}
}

std::string PeerNetwork::get_client_ip_list(){
	std::string list;
	for(int i=0;i<this->clients.size();i++){
		list = list + this->clients[i].getIp() + '\n';
	}
	return list;
}

int PeerNetwork::get_client_ip_list_int(uint32_t* buf, int size){
	for(int i=0;(i<this->clients.size()) && (i*4<size);i++){
		buf[i] = this->clients[i].getIpInt();
	}
	return this->clients.size()*4;
}