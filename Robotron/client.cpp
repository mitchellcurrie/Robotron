//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: client.cpp
// Description	: implementation file
// Author		: Mitchell Currie
// Mail			: Mitchell.Currie@mediadesign.school.nz
//

//Library Includes
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <ctime>

//Local Includes
#include "consoletools.h"
#include "network.h"
#include "networkentity.h"
#include "socket.h"

//This includes
#include "client.h"

CClient::CClient()
	:m_pcPacketData(0)
	, m_pClientSocket(0)
	, m_bGameStarted(false)
{
	ZeroMemory(&m_ServerSocketAddress, sizeof(m_ServerSocketAddress));

	//Create a Packet Array and fill it out with all zeros.
	m_pcPacketData = new char[MAX_MESSAGE_LENGTH];
	ZeroMemory(m_pcPacketData, MAX_MESSAGE_LENGTH);
	bool m_bQuit = false;

}

CClient::~CClient()
{
	delete[] m_pcPacketData;
	m_pcPacketData = 0;

	delete m_pClientSocket;
	m_pClientSocket = 0;

	delete m_pWorkQueue;
	m_pWorkQueue = 0;
}

/***********************
* Initialise: Initialises a client object by creating a client socket and filling out the socket address structure with details of server to send the data to.
* @author: 
* @parameter: none
* @return: void
********************/
bool CClient::Initialise()
{
	//Local Variables to hold Server's IP address and Port NUmber as entered by the user
	char _cServerIPAddress[128];
	ZeroMemory(&_cServerIPAddress, 128);
	char _cServerPort[10];
	ZeroMemory(&_cServerPort, 10);
	unsigned short _usServerPort;

	char _cServerChosen[5];
	ZeroMemory(&_cServerChosen, 5);
	unsigned int _uiServerIndex;

	//Local variable to hold client's name
	char _cUserName[50];
	ZeroMemory(&m_cUserName, 50);

	//Zero out the memory for all the member variables.
	ZeroMemory(&m_cUserName, strlen(m_cUserName));


	//Create a work queue to distribute messages between the main  thread and the receive thread.
	m_pWorkQueue = new CWorkQueue<char*>();

	//Create a socket object
	m_pClientSocket = new CSocket();

	//Get the port number to bind the socket to
	unsigned short _usClientPort = QueryPortNumber(DEFAULT_CLIENT_PORT);
	//Initialise the socket to the local loop back address and port number
	if (!m_pClientSocket->Initialise("127.0.0.1", _usClientPort))
	{
		return false;
	}

	//Set the client's online status to true
	m_bOnline = true;

	//Use a boolean flag to determine if a valis server has been chosen by the client or not
	bool _bServerChosen = false;

	do{
#pragma region _GETSERVER_
		unsigned char _ucChoice = QueryOption("Do you want to broadcast for servers or manually connect (B/M)?", "BM");

		switch (_ucChoice)
		{
		case 'B':
		{
			//Question 7: Broadcast to detect server
			m_bDoBroadcast = true;
			m_pClientSocket->EnableBroadcast();
			BroadcastForServers();
			if (m_vecServerAddr.size() == 0)
			{
				std::cout << "No Servers Found " << std::endl;
				continue;
			}
			else{

				//Give a list of servers for the user to choose from :
				for (unsigned int i = 0; i < m_vecServerAddr.size(); i++)
				{
					inet_ntop(AF_INET, &(m_vecServerAddr[i].sin_addr), _cServerIPAddress, MAX_ADDRESS_LENGTH);
					std::cout << std::endl << "[" << i << "]" << " SERVER : found at " << _cServerIPAddress << ": " << ntohs(m_vecServerAddr[i].sin_port) << std::endl;
				}
				std::cout << std::endl;
				std::cout << "Choose a server number to connect to :";
				gets_s(_cServerChosen);

				_uiServerIndex = atoi(_cServerChosen);
				m_ServerSocketAddress.sin_family = AF_INET;
				m_ServerSocketAddress.sin_port = m_vecServerAddr[_uiServerIndex].sin_port;
				m_ServerSocketAddress.sin_addr.S_un.S_addr = m_vecServerAddr[_uiServerIndex].sin_addr.S_un.S_addr;
				//inet_pton(AF_INET, _cServerIPAddress, &m_vecServerAddr[_uiServerIndex].sin_addr);
				inet_ntop(AF_INET, &(m_vecServerAddr[_uiServerIndex].sin_addr), _cServerIPAddress, MAX_ADDRESS_LENGTH);
				std::cout << "Attempting to connect to server at " << _cServerIPAddress << ":" << ntohs(m_vecServerAddr[_uiServerIndex].sin_port) << std::endl;
				_bServerChosen = true;
			}
			m_bDoBroadcast = false;
			m_pClientSocket->DisableBroadcast();
			break;
		}
		case 'M':
		{
			std::cout << "Enter server IP or empty for localhost: ";

			gets_s(_cServerIPAddress);
			if (_cServerIPAddress[0] == 0)
			{
				strcpy_s(_cServerIPAddress, "127.0.0.1");
			}
			//Get the Port Number of the server
			std::cout << "Enter server's port number or empty for default server port: ";
			gets_s(_cServerPort);
			//std::cin >> _usServerPort;

			if (_cServerPort[0] == 0)
			{
				_usServerPort = DEFAULT_SERVER_PORT;
			}
			else
			{
				_usServerPort = atoi(_cServerPort);
			}
			//Fill in the details of the server's socket address structure.
			//This will be used when stamping address on outgoing packets
			m_ServerSocketAddress.sin_family = AF_INET;
			m_ServerSocketAddress.sin_port = htons(_usServerPort);
			inet_pton(AF_INET, _cServerIPAddress, &m_ServerSocketAddress.sin_addr);
			_bServerChosen = true;
			std::cout << "Attempting to connect to server at " << _cServerIPAddress << ":" << _usServerPort << std::endl;
			break;
		}
		default:
		{
			std::cout << "This is not a valid option" << std::endl;
			return false;
			break;
		}
		}
#pragma endregion _GETSERVER_

	} while (_bServerChosen == false);
	//Fill in the details of the server's socket address structure.
	//This will be used when stamping address on outgoing packets
	//m_ServerSocketAddress.sin_family = AF_INET;
	//m_ServerSocketAddress.sin_port = htons(_usServerPort);
	//inet_pton(AF_INET, _cServerIPAddress, &m_ServerSocketAddress.sin_addr);

	//Send a hanshake message to the server as part of the Client's Initialization process.
	//Step1: Create a handshake packet

	/*do{
		std::cout << "Please enter a username : ";
		gets_s(_cUserName);
	} while (_cUserName[0] == 0);

	strcpy_s(m_cUserName, strlen(_cUserName) + 1, _cUserName);
*/
//	std::cout << "Press '!?' to bring up the Command List." << std::endl;


	TPacket _packet;
	_packet.Serialize(HANDSHAKE, m_cUserName, m_cUserName); //Hardcoded username; change to name as taken in via user input.
	SendData(_packet.PacketData);
	return true;
}

bool CClient::SendData(char* _pcDataToSend)
{
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;
	
	char _RemoteIP[MAX_ADDRESS_LENGTH];
	inet_ntop(AF_INET, &m_ServerSocketAddress.sin_addr, _RemoteIP, sizeof(_RemoteIP));
	//std::cout << "Trying to send " << _pcDataToSend << " to " << _RemoteIP << ":" << ntohs(m_ServerSocketAddress.sin_port) << std::endl;
	char _message[MAX_MESSAGE_LENGTH];
	strcpy_s(_message, strlen(_pcDataToSend) + 1, _pcDataToSend);

	int iNumBytes = sendto(
		m_pClientSocket->GetSocketHandle(),				// socket to send through.
		_pcDataToSend,									// data to send
		_iBytesToSend,									// number of bytes to send
		0,												// flags
		reinterpret_cast<sockaddr*>(&m_ServerSocketAddress),	// address to be filled with packet target
		sizeof(m_ServerSocketAddress)							// size of the above address struct.
		);
	//iNumBytes;
	if (_iBytesToSend != iNumBytes)
	{
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}

bool CClient::SendDataTo(char* _pcDataToSend, sockaddr_in Address)
{
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	char _RemoteIP[MAX_ADDRESS_LENGTH];
	inet_ntop(AF_INET, &Address.sin_addr, _RemoteIP, sizeof(_RemoteIP));
	//std::cout << "Trying to send " << _pcDataToSend << " to " << _RemoteIP << ":" << ntohs(m_ServerSocketAddress.sin_port) << std::endl;
	char _message[MAX_MESSAGE_LENGTH];
	strcpy_s(_message, strlen(_pcDataToSend) + 1, _pcDataToSend);

	int iNumBytes = sendto(
		m_pClientSocket->GetSocketHandle(),				// socket to send through.
		_pcDataToSend,									// data to send
		_iBytesToSend,									// number of bytes to send
		0,												// flags
		reinterpret_cast<sockaddr*>(&Address),	// address to be filled with packet target
		sizeof(Address)							// size of the above address struct.
		);
	//iNumBytes;
	if (_iBytesToSend != iNumBytes)
	{
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}

void CClient::ReceiveData(char* _pcBufferToReceiveData)
{
	//set off the timer previuosly set for broadcast message
	struct timeval timeValue;
	timeValue.tv_sec = 0;
	timeValue.tv_usec = 0;
	setsockopt(m_pClientSocket->GetSocketHandle(), SOL_SOCKET, SO_RCVTIMEO, (char*)&timeValue, sizeof(timeValue));

	sockaddr_in _FromAddress; // Make a local variable to extract the IP and port number of the sender from whom we are receiving
	//In this case; it should be the details of the server; since the client only ever receives from the server
	int iSizeOfAdd = sizeof(_FromAddress);
	int _iNumOfBytesReceived;
	int _iPacketSize;
	
	//Receive data into a local buffer
	char _buffer[MAX_MESSAGE_LENGTH];
	//For debugging purpose only, convert the Address structure to a string.
	char _pcAddress[50];
	ZeroMemory(&_pcAddress, 50);

	while(m_bOnline)   // Changed from while true

	{
		// pull off the packet(s) using recvfrom()
		_iNumOfBytesReceived = recvfrom(			// pulls a packet from a single source...
			this->m_pClientSocket->GetSocketHandle(),						// client-end socket being used to read from
			_buffer,							// incoming packet to be filled
			MAX_MESSAGE_LENGTH,					   // length of incoming packet to be filled
			0,										// flags
			reinterpret_cast<sockaddr*>(&_FromAddress),	// address to be filled with packet source
			&iSizeOfAdd								// size of the above address struct.
			);
		inet_ntop(AF_INET, &_FromAddress, _pcAddress, sizeof(_pcAddress));


		if (_iNumOfBytesReceived < 0)
		{
			int _iError = WSAGetLastError();

			if (_iError == WSAECONNRESET)
			{
			
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

				TPacket _packet;
				_packet.Serialize(DATA, "Error", "Server dropped. Please close the window.");
				// SendDataTo(_packet.PacketData, m_ServerSocketAddress);

				/*TPacket _packet;
				_packet.Serialize(QUIT, "Blank", "Blank");
				Process(_packet.PacketData);*/

				m_pWorkQueue->push(_packet.PacketData);

				//std::cout << "Server dropped. Please close the window." << std::endl;
				//m_bOnline = false;

				// Instead could send a quit or modified quit message informing the client of the server drop, and quitting them out, without making m_bOnline false
			}
			else
			{
				//Error in receiving data 
				std::cout << "recvfrom failed with error " << WSAGetLastError();
			}
			
			//_pcBufferToReceiveData = 0;
			m_pcPacketData = 0;
		}
		else if (_iNumOfBytesReceived == 0)
		{
			//The remote end has shutdown the connection
			// _pcBufferToReceiveData = 0;
			m_pcPacketData = 0;
		}
		else
		{
			// Old //
			////There is valid data received.
			////Put this packet data in the workQ
			//_pcBufferToReceiveData = _buffer;
			//
			//m_pWorkQueue->push(_pcBufferToReceiveData);

			/// NEW ///
			//There is valid data received.
			/*strcpy_s(_pcBufferToReceiveData, strlen(_buffer) + 1, _buffer);
			m_ServerSocketAddress = _FromAddress;
			m_pWorkQueue->push(_pcBufferToReceiveData);*/

			strcpy_s(m_pcPacketData, strlen(_buffer) + 1, _buffer);
			m_ServerSocketAddress = _FromAddress;
			m_pWorkQueue->push(m_pcPacketData);
		}
	
		std::this_thread::yield(); //Yield the processor; giving the main a chance to run.
	}
}

void CClient::ReceiveBroadcastMessages(char* _pcBufferToReceiveData)
{
	//set a timer on the socket for one second
	struct timeval timeValue;
	timeValue.tv_sec = 1;
	timeValue.tv_usec = 0;
	setsockopt(m_pClientSocket->GetSocketHandle(), SOL_SOCKET, SO_RCVTIMEO, (char*)&timeValue, sizeof(timeValue));

	//Receive data into a local buffer
	char _buffer[MAX_MESSAGE_LENGTH];
	sockaddr_in _FromAddress;
	int iSizeOfAdd = sizeof(sockaddr_in);
	//char _pcAddress[50];

	while (m_bDoBroadcast)
	{
		// pull off the packet(s) using recvfrom()
		int _iNumOfBytesReceived = recvfrom(				// pulls a packet from a single source...
			this->m_pClientSocket->GetSocketHandle(),	// client-end socket being used to read from
			_buffer,									// incoming packet to be filled
			MAX_MESSAGE_LENGTH,							// length of incoming packet to be filled
			0,											// flags
			reinterpret_cast<sockaddr*>(&_FromAddress),	// address to be filled with packet source
			&iSizeOfAdd								// size of the above address struct.
			);

		if (_iNumOfBytesReceived < 0)
		{
			//Error in receiving data 
			int _iError = WSAGetLastError();
			//std::cout << "recvfrom failed with error " << _iError;
			if (_iError == WSAETIMEDOUT) // Socket timed out on Receive
			{
				m_bDoBroadcast = false; //Do not broadcast any more
				break;
			}
			_pcBufferToReceiveData = 0;
		}
		else if (_iNumOfBytesReceived == 0)
		{
			//The remote end has shutdown the connection
			_pcBufferToReceiveData = 0;
		}
		else
		{
			//There is valid data received.
			strcpy_s(_pcBufferToReceiveData, strlen(_buffer) + 1, _buffer);
			m_ServerSocketAddress = _FromAddress;
			m_vecServerAddr.push_back(m_ServerSocketAddress);
		}
	}//End of while loop
}

void CClient::GetRemoteIPAddress(char *_pcSendersIP)
{
	inet_ntop(AF_INET, &(m_ServerSocketAddress.sin_addr), _pcSendersIP, sizeof(_pcSendersIP));
	return;
}

unsigned short CClient::GetRemotePort()
{
	return ntohs(m_ServerSocketAddress.sin_port);
}

void CClient::GetPacketData(char* _pcLocalBuffer)
{
	strcpy_s(_pcLocalBuffer, strlen(m_pcPacketData) + 1, m_pcPacketData);
}

CWorkQueue<char*>* CClient::GetWorkQueue()
{
	return m_pWorkQueue;
}

char* CClient::GetName()
{
	return m_cUserName;
}

void CClient::Process (char* BufferToReceive)
{
	TPacket _packetRecvd = _packetRecvd.Deserialize(BufferToReceive);
	TPacket _packetSent;
	
	switch (_packetRecvd.MessageType)
	{
	case BROADCAST:
	{
		//std::cout << "Received a broadcast packet" << std::endl;
		//Extract the server's details from the packet 
		m_vecServerAddr.push_back(m_ServerSocketAddress);
		break;
	}
	case KEEPALIVE:
	{
		//Extract the server's details from the packet 
		// std::cout << "Received keep alive message" << std::endl;

	/*	TPacket _packet;
		_packet.Serialize(KEEPALIVE, m_cUserName, " ");
		SendData(_packet.PacketData);
		break;*/
	}
	case HANDSHAKE:
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

		std::string Content = _strdup(_packetRecvd.MessageContent);

		//strcpy_s(Content, _packetRecvd.MessageContent, _packetRecvd.MessageContent);

		if (Content == " DENIED")
		{
			std::cout << "Already 4 Players!" << std::endl;

			while (true)
			{

			}
		}
		else
		{
			strcpy_s(m_cUserName, strlen(_packetRecvd.MessageContent) + 1, _packetRecvd.MessageContent);
		}

		break;
	}
	case COMMAND:
	{
		char Command = _packetRecvd.MessageContent[1];

		switch (Command)
		{
			case 'G':
			{
				m_bGameStarted = true;
				break;
			}

			case 'P':
			{
				// Message received from main to send position to server
				// Add another layer to serialize/deserialize after message content, after the "P"
				
				// Send a packet to the server with the clients position
				// m_pModel->GetPosition()   etc

				_packetSent.Serialize(COMMAND, "SERVER", "P");
				SendData(_packetSent.PacketData);

				break;
			}

			default:
			{
				break;
			}
		}

		//SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
		////	std::cout << "Server Received " << _pcBufferToReceiveData << " from " << _IPAddress << " :" << ntohs(m_ClientAddress.sin_port) << std::endl;
		////	std::cout << "Server received " << _packetRecvd.MessageContent << " from " << _packetRecvd.Name << std::endl;
		//std::cout << "Command List:" << std::endl;
		//std::cout << "'!Q' to Quit" << std::endl;
		//break;
	}
	case QUIT:
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
		/*std::cout << "Server dropped. Please close the window." << std::endl;
		m_bOnline = false;*/

		/*#include <cstdlib>
		std::exit(exit_code);*/
		// system("CLS");

		// std::cout << "You have left the chat. Please close the window." << std::endl;
		//std::cout << "hitting case quit" << std::endl;

		std::cout << _packetRecvd.Name << ":" << _packetRecvd.MessageContent << std::endl;

		//while (true)
		//{
		//	// Left the chat
		//}

		break;
	}
	case DATA:
	{
		//	std::cout << "Server Received " << _pcBufferToReceiveData << " from " << _IPAddress << " :" << ntohs(m_ClientAddress.sin_port) << std::endl;
		//	std::cout << "Server received " << _packetRecvd.MessageContent << " from " << _packetRecvd.Name << std::endl;
		std::cout << _packetRecvd.Name << ":" << _packetRecvd.MessageContent << std::endl;
		break;
	}

	default:
		break;

	}

}

bool CClient::BroadcastForServers()
{
	//Make a broadcast packet
	TPacket _packet;
	_packet.Serialize(BROADCAST, m_cUserName, "DetectServer");
	char _temp[MAX_MESSAGE_LENGTH];

	//Send out a broadcast message using the broadcast address
	m_pClientSocket->SetRemoteAddress(INADDR_BROADCAST);
	m_pClientSocket->SetRemotePort(DEFAULT_SERVER_PORT);

	m_ServerSocketAddress.sin_family = AF_INET;
	m_ServerSocketAddress.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	for (int i = 0; i < 10; i++) //Just try  a series of 10 ports to detect a runmning server; this is needed since we are testing multiple servers on the same local machine
	{
		m_ServerSocketAddress.sin_port = htons(DEFAULT_SERVER_PORT + i);
		SendData(_packet.PacketData);
	}
	ReceiveBroadcastMessages(_temp);

	return true;
}

bool CClient::HasGameStarted()
{
	return m_bGameStarted;
}