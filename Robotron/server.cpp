//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: server.cpp
// Description	: implementation file
// Author		: Mitchell Currie
// Mail			: Mitchell.Currie@mediadesign.school.nz
//

//Library Includes
#include <WS2tcpip.h>
#include <iostream>
#include <utility>
#include <thread>
#include <vector>
//#include <mutex>

//Local Includes
#include "network.h"
#include "consoletools.h"
#include "socket.h"


//Local Includes
#include "server.h"

std::string CServer::m_sNewestClientName = "";
std::clock_t CServer::m_start;
double CServer::m_duration = 0;
int CServer::m_iNumberOfConnectedPlayers = 0;
// std::mutex mutexlock;

CServer::CServer()
	:m_pcPacketData(0),
	m_pServerSocket(0),
	m_bGameStarted(true),
	m_bNotifiedPlayersOfStart(false)
{
	ZeroMemory(&m_ClientAddress, sizeof(m_ClientAddress));
}

CServer::~CServer()
{
	delete[] m_pcPacketData;
	m_pcPacketData = 0;
	
	delete m_pServerSocket;
	m_pServerSocket = 0;
}

bool CServer::Initialise()
{
	//Create a work queue to distribute messages between the main  thread and the receive thread.
	m_pWorkQueue = new CWorkQueue<char*>();

	m_pcPacketData = new char[MAX_MESSAGE_LENGTH];
	
	//Create a socket object
	m_pServerSocket = new CSocket();

	//Get the port number to bind the socket to
	unsigned short _usServerPort = QueryPortNumber(DEFAULT_SERVER_PORT);

	//Initialise the socket to the local loop back address and port number
	if (!m_pServerSocket->Initialise("127.0.0.1", _usServerPort))
	{
		return false;
	}

	//Set the servers's online status to true
	m_bOnline = true;

	//Qs 2: Create the map to hold details of all connected clients
	//m_pConnectedClients = new std::map < std::string, TClientDetails>;

	m_start = std::clock();

	return true;
}

bool CServer::AddClient(TClientDetails _clientDetails)
{
	//std::map<std::string, TClientDetails>::iterator it;

	TPacket _packet;

	if (m_iNumberOfConnectedPlayers == 0)
	{
		_packet.Serialize(HANDSHAKE, "Server", "P1");
		m_pConnectedClients.insert(std::pair<std::string, TClientDetails>("P1", _clientDetails));
		m_sNewestClientName = "P1";
		m_iNumberOfConnectedPlayers = 1;
		std::cout << "Received a handshake message " << "from " << "P1" << std::endl;
	}

	else if (m_iNumberOfConnectedPlayers == 1)
	{
		_packet.Serialize(HANDSHAKE, "Server", "P2");
		m_pConnectedClients.insert(std::pair<std::string, TClientDetails>("P2", _clientDetails));
		m_sNewestClientName = "P2";
		m_iNumberOfConnectedPlayers = 2; 
		std::cout << "Received a handshake message " << "from " << "P2" << std::endl;
	}

	else if (m_iNumberOfConnectedPlayers == 2)
	{
		_packet.Serialize(HANDSHAKE, "Server", "P3");
		m_pConnectedClients.insert(std::pair<std::string, TClientDetails>("P3", _clientDetails));
		m_sNewestClientName = "P3";
		m_iNumberOfConnectedPlayers = 3;
		std::cout << "Received a handshake message " << "from " << "P3" << std::endl;
	}

	else if (m_iNumberOfConnectedPlayers == 3)
	{
		_packet.Serialize(HANDSHAKE, "Server", "P4");
		m_pConnectedClients.insert(std::pair<std::string, TClientDetails>("P4", _clientDetails));
		m_sNewestClientName = "P4";
		m_iNumberOfConnectedPlayers = 4;
		std::cout << "Received a handshake message " << "from " << "P4" << std::endl;
	}

	else
	{
		return false;
	}
				
	SendDataTo(_packet.PacketData, _clientDetails.m_ClientAddress);

	//SendData("1 Server Handshake_accepted");
	return true;
	
}

bool CServer::SendData(char* _pcDataToSend)
{
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	char _RemoteIP[MAX_ADDRESS_LENGTH];
	inet_ntop(AF_INET, &m_ClientAddress.sin_addr, _RemoteIP, sizeof(_RemoteIP));
	//std::cout << "Trying to send " << _pcDataToSend << " to " << _RemoteIP << ":" << ntohs(m_ServerSocketAddress.sin_port) << std::endl;
	char _message[MAX_MESSAGE_LENGTH];
	strcpy_s(_message, strlen(_pcDataToSend) + 1, _pcDataToSend);

	
	int iNumBytes = sendto(
		m_pServerSocket->GetSocketHandle(),				// socket to send through.
		_pcDataToSend,									// data to send
		_iBytesToSend,									// number of bytes to send
		0,												// flags
		reinterpret_cast<sockaddr*>(&m_ClientAddress),	// address to be filled with packet target
		sizeof(m_ClientAddress)							// size of the above address struct.
		);
	//iNumBytes;
	if (_iBytesToSend != iNumBytes)
	{
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}


bool CServer::SendDataTo(char* _pcDataToSend, sockaddr_in Address)
{
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	char _RemoteIP[MAX_ADDRESS_LENGTH];
	//inet_ntop(AF_INET, &Address.sin_addr, _RemoteIP, sizeof(_RemoteIP));
	//std::cout << "Trying to send " << _pcDataToSend << " to " << _RemoteIP << ":" << ntohs(Address.sin_port) << std::endl;
	char _message[MAX_MESSAGE_LENGTH];
	strcpy_s(_message, strlen(_pcDataToSend) + 1, _pcDataToSend);

	int iNumBytes = sendto(
		m_pServerSocket->GetSocketHandle(),				// socket to send through.
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

bool CServer::SendDataToAll(char* _pcDataToSend)
{
	int _iBytesToSend;

	int iNumBytes;

	for (auto it = m_pConnectedClients.begin(); it != m_pConnectedClients.end(); it++) {

		_iBytesToSend = (int)strlen(_pcDataToSend) + 1;

		iNumBytes = sendto(
			m_pServerSocket->GetSocketHandle(),			// socket to send through.
			_pcDataToSend,								// data to send
			_iBytesToSend,								// number of bytes to send
			0,											// flags
			reinterpret_cast<sockaddr*>(&it->second),	// address to be filled with packet target
			sizeof(it->second)							// size of the above address struct.
			);

		if (_iBytesToSend != iNumBytes) {
			std::cout << "There was an error in sending data from client to server" << std::endl;
			return false;
		}
	}

	return true;
}

void CServer::ReceiveData(char* _pcBufferToReceiveData)
{
	int iSizeOfAdd = sizeof(m_ClientAddress);
	int _iNumOfBytesReceived;
	int _iPacketSize;

	while (m_bOnline)
	{
		// pull off the packet(s) using recvfrom()
		_iNumOfBytesReceived = recvfrom(			// pulls a packet from a single source...
			m_pServerSocket->GetSocketHandle(),						// client-end socket being used to read from
			m_pcPacketData,							// incoming packet to be filled
			MAX_MESSAGE_LENGTH,					   // length of incoming packet to be filled
			0,										// flags
			reinterpret_cast<sockaddr*>(&m_ClientAddress),	// address to be filled with packet source
			&iSizeOfAdd								// size of the above address struct.
			);

		if (_iNumOfBytesReceived < 0)
		{
			int _iError = WSAGetLastError();
			if (_iError == WSAECONNRESET)
			{

			}
			else
			{
				ErrorRoutines::PrintWSAErrorInfo(_iError);
			}
			//return false;
		}

		else if (_iNumOfBytesReceived == 0)
		{
			//The remote end has shutdown the connection
			_pcBufferToReceiveData = 0;
		}

		else
		{
			_iPacketSize = strlen(m_pcPacketData) + 1;
			strcpy_s(_pcBufferToReceiveData, _iPacketSize, m_pcPacketData);
			//std::cout << "Server Received " << _pcBufferToReceiveData << "from " << ntohs(m_ClientAddress.sin_port) << std::endl;
			char _IPAddress[100];
			inet_ntop(AF_INET, &m_ClientAddress, _IPAddress, sizeof(_IPAddress));

			TPacket _packetRecvd = _packetRecvd.Deserialize(_pcBufferToReceiveData);

			switch (_packetRecvd.MessageType)
			{
				case BROADCAST:
				{
					//std::cout << "Received a broadcast packet" << std::endl;
					break;
				}
				case HANDSHAKE:
				{
				//	std::cout << "Received a handshake message " << "from " << _packetRecvd.Name << std::endl;
					break;
				}
				case DATA:
				{
					//	std::cout << "Server Received " << _pcBufferToReceiveData << " from " << _IPAddress << " :" << ntohs(m_ClientAddress.sin_port) << std::endl;
					std::cout << "Received " << _packetRecvd.MessageContent << " from " << _packetRecvd.Name << std::endl;
					break;
				}

				default:
					break;
			}

			m_pWorkQueue->push(_pcBufferToReceiveData); // Here or at start of else statement?
		}

		std::this_thread::yield(); // Need this?

	}

	//std::cout << "Server Received " << _pcBufferToReceiveData << "from " << _IPAddress << " :" << ntohs(m_ClientAddress.sin_port) << std::endl;
	//std::cout << "Server Received " << _pcBufferToReceiveData << " from " << _IPAddress << " :" << ntohs(m_ClientAddress.sin_port) << std::endl;
}

void CServer::GetRemoteIPAddress(char *_pcSendersIP)
{
	char _temp[MAX_ADDRESS_LENGTH];
	int _iAddressLength;
	inet_ntop(AF_INET, &(m_ClientAddress.sin_addr), _temp, sizeof(_temp));
	_iAddressLength = strlen(_temp) + 1;
	strcpy_s(_pcSendersIP, _iAddressLength, _temp);
}

unsigned short CServer::GetRemotePort()
{
	return ntohs(m_ClientAddress.sin_port);
}

void CServer::ProcessData(char* _pcDataReceived)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);

	TPacket _packetRecvd, _packetSent;
	_packetRecvd = _packetRecvd.Deserialize(_pcDataReceived);

	TClientDetails ClientDetails;
	ClientDetails.m_ClientAddress = m_ClientAddress;
	ClientDetails.m_strName = _packetRecvd.Name;
	std::string strClientName = _packetRecvd.Name;

	switch (_packetRecvd.MessageType)
	{
	case BROADCAST:
	{
		//std::cout << "Received a broadcast packet" << std::endl;
		//Just send out a packet to the back to the client again which will have the server's IP and port in it's sender fields
		_packetSent.Serialize(BROADCAST, _packetRecvd.Name, "I'm here!");
		SendData(_packetSent.PacketData);
		break;
	}

	case COMMAND:
	{
		char Command = _packetRecvd.MessageContent[1];

		switch (Command)
		{
			case 'G':
			{
				_packetSent.Serialize(COMMAND, "SERVER", "G");
				SendDataToAll(_packetSent.PacketData);
				break;
			}

			case 'P':
			{
				// Receive positions from client
				// Update them in the vector of players

//				int x = 5;
				break;
			}

			default:
			{
				break;
			}
		}
		
		break;
	}
	case KEEPALIVE:
	{
//		if (strClientName == "Server") // Time message to send out keep alive messages
//		{
//			for (auto itr : m_pConnectedClients)
//			{
//				_packetSent.Serialize(KEEPALIVE, "Server", " ");
//				SendDataTo(_packetSent.PacketData, itr.second.m_ClientAddress);
//			}
//		}
//
//		else // Messages back from the clients
//		{
//			// std::cout << "Received keep alive message back from ";
//			// std::cout << strClientName << std::endl;
//
//			bool bAlreadyAdded = false;
//
//			for (auto itr3 : m_VecNames)
//			{
//				if (itr3 == strClientName)
//				{
//					bAlreadyAdded = true;
//				}
//			}
//
//			if (!bAlreadyAdded)
//				m_VecNames.push_back(strClientName);
//
//		}
//
////		mutexlock.lock();
//
//		m_duration = (std::clock() - m_start) / (double)CLOCKS_PER_SEC;
//
//		bool bReceivedMessage = false;
//
//		if ((m_duration > 1) && (!m_pConnectedClients.empty()))
//		{
//			for (auto itr : m_pConnectedClients)
//			{
//				for (auto itr2 : m_VecNames)
//				{
//					if (itr.second.m_strName == itr2)
//					{
//						bReceivedMessage = true;
//					}					
//				}	
//
//				if (!bReceivedMessage)
//				{
//					/*TPacket _packet;*/
//
//					char* _pcName = _strdup(itr.second.m_strName.c_str());
//
//					m_Quitters.push_back(_pcName);
//
//					//_packet.Serialize(QUIT, _pcName, "No Message"); // Send quit message to server
//					//ProcessData(_packet.PacketData);
//
//					//if (m_pConnectedClients.empty()) // Otherwise error because we are iterating over a now empty map
//					//{
//					//	m_start = std::clock();
//					//	m_VecNames.clear();
//					//	break;
//					//}
//				}
//
//				bReceivedMessage = false;
//			}
//
//			if (!m_Quitters.empty())
//			{
//				TPacket _packet;
//
//				for (auto itr : m_Quitters)
//				{
//					//_packet.Serialize(QUIT, itr, "No Message"); // Send quit message to server
//					//ProcessData(_packet.PacketData);
//					//m_pWorkQueue->push(_packet.PacketData);
//
//					std::cout << "Received a quit message from ";
//					std::cout << itr << std::endl;
//
//					// Remove client from map of clients
//					m_pConnectedClients.erase(itr);
//
//					std::string Message = itr;
//					Message += " has left the chat";
//
//					char* CharMessage = &Message[0u];
//
//					_packet.Serialize(QUIT, "Server", CharMessage);
//
//					// Notify other users
//					for (auto itr2 : m_pConnectedClients)
//					{
//						SendDataTo(_packet.PacketData, itr2.second.m_ClientAddress);
//						//std::cout << "Sent data to: " << itr2.second.m_strName << std::endl;
//					}
//				}	
//			}
//
//			m_start = std::clock();
//
//			m_VecNames.clear();
//			m_Quitters.clear();
//		}
//
////		mutexlock.unlock();
//			
//		break;
	}
	case HANDSHAKE:
	{
		if (!(AddClient(ClientDetails)))
		{
			_packetSent.Serialize(HANDSHAKE, "SERVER", "DENIED");
			SendDataTo(_packetSent.PacketData, m_ClientAddress);
		}

	//	std::cout << "Server received a handshake message " << "from " << strClientName << std::endl;
	//	if (AddClient(ClientDetails))
	//	{
			//std::string Message = "1 Server ";
			//Message += m_sNewestClientName;
			//Message += " joined the chat. ";
		//	Message += "Current users: ";

			//for (auto itr : m_pConnectedClients)
			//{
			//	Message += itr.second.m_strName;
			//	Message += ", ";
			//}

			//std::string UpdatedMessage = Message.substr(0, Message.length() - 2); // to remove the comma at the end 

			//char* CharMessage = &UpdatedMessage[0u];
			/*char* CharMessage = new char[Message.size() + 1];
			std::copy(Message.begin(), Message.end(), CharMessage);*/
		//	std::vector<char> CharMessage(Message.begin(), Message.end());
		//	CharMessage.push_back('\0');

			
			//for (auto itr : m_pConnectedClients)
			//{
			//	// SendDataTo(&CharMessage[0], itr.second.m_ClientAddress);
			//	SendDataTo(CharMessage, itr.second.m_ClientAddress);
			//	// std::cout << "Sent data to: handshake" << std::endl;
			//}	

			//m_start = std::clock();
	//	}

		break;
	}

	case QUIT:
	{
		std::cout << "Received a quit message from ";
		std::cout << _packetRecvd.Name << std::endl;

		// Remove client from map of clients
		m_pConnectedClients.erase(_packetRecvd.Name);

		std::string Message = "5 Server ";
		Message += _packetRecvd.Name;
		Message += " has left the chat";

		char* CharMessage = &Message[0u];

		// Notify other users
		for (auto itr : m_pConnectedClients)
		{
			SendDataTo(CharMessage, itr.second.m_ClientAddress);
		//	std::cout << "Sent data to: " << itr.second.m_strName << std::endl;
		}

		// User quit
		//SendDataTo("5", m_ClientAddress);

		break;
	}



	case DATA:
	{
		_packetSent.Serialize(DATA, _packetRecvd.Name, _packetRecvd.MessageContent);
		// SendData(_packetSent.PacketData); // Send data needs to send to everyone

		//if ((_packetRecvd.MessageContent[1] == '!') && (_packetRecvd.MessageContent[2] == '?'))
		//{
		//	// Command menu
		//	SendDataTo("3", m_ClientAddress);
		////	std::cout << "Sent data - data: " << std::endl;
		//}

		//else if ((_packetRecvd.MessageContent[1] == '!') && (_packetRecvd.MessageContent[2] == '!'))
		//{
		//	_packetRecvd.MessageContent[1] = ' ';
		//	_packetSent.Serialize(DATA, _packetRecvd.Name, _packetRecvd.MessageContent);

		//	for (auto itr : m_pConnectedClients)
		//	{
		//		SendDataTo(_packetSent.PacketData, itr.second.m_ClientAddress);
		//	//	std::cout << "Sent data - data: " << std::endl;
		//		//			SendData(_packetSent.PacketData);
		//		//itr.second.m_ClientAddress
		//	}
		//}

		//else if (_packetRecvd.MessageContent[1] == '!')
		//{
		//	SendDataTo("1 Server Command does not exist", m_ClientAddress);
		//}

	//	else
	//	{				
			for (auto itr : m_pConnectedClients)
			{
				SendDataTo(_packetSent.PacketData, itr.second.m_ClientAddress);
			//	std::cout << "Sent data to data ELSE: " << itr.second.m_strName << std::endl;
				//			SendData(_packetSent.PacketData);
				//itr.second.m_ClientAddress
			}
//		}
		break;
	}

	default:
		break;

	}
}

void CServer::GetPacketData(char* _pcLocalBuffer)
{
	strcpy_s(_pcLocalBuffer, strlen(m_pcPacketData) + 1, m_pcPacketData);
}

CWorkQueue<char*>* CServer::GetWorkQueue()
{
	return m_pWorkQueue;
}

bool CServer::HasGameStarted()
{
	return m_bGameStarted;
}

bool CServer::HasNotifiedPlayers()
{
	return m_bNotifiedPlayersOfStart;
}

void CServer::SetNotifiedPlayers(bool _b)
{
	m_bNotifiedPlayersOfStart = _b;
}

int CServer::GetNumberOfPlayers()
{
	return m_iNumberOfConnectedPlayers;
}
