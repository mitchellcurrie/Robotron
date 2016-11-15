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
#include "utils.h"
#include "global.h"

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
	m_bNotifiedPlayersOfStart(false),
	m_iActivePlayers(0) {
	ZeroMemory(&m_ClientAddress, sizeof(m_ClientAddress));
}

CServer::~CServer() {
	delete[] m_pcPacketData;
	m_pcPacketData = 0;

	delete m_pServerSocket;
	m_pServerSocket = 0;
}

bool CServer::Initialise() {
	//Create a work queue to distribute messages between the main  thread and the receive thread.
	m_pWorkQueue = new CWorkQueue<char*>();

	m_pcPacketData = new char[MAX_MESSAGE_LENGTH];

	//Create a socket object
	m_pServerSocket = new CSocket();

	//Get the port number to bind the socket to
	//unsigned short _usServerPort = QueryPortNumber(DEFAULT_SERVER_PORT);

	//Initialise the socket to the local loop back address and port number
	if (!m_pServerSocket->Initialise("127.0.0.1", DEFAULT_SERVER_PORT)) {
		return false;
	}

	//Set the servers's online status to true
	m_bOnline = true;

	//Qs 2: Create the map to hold details of all connected clients
	//m_connectedClients = new std::map < std::string, TClientDetails>;

	m_start = std::clock();

	return true;
}

bool CServer::AddClient(TClientDetails _clientDetails) {

	TPacket _packet;

	if (m_iNumberOfConnectedPlayers == 0) {
		_packet.Serialize(HANDSHAKE, "Server", "P1");
		m_connectedClients.insert(std::pair<std::string, TClientDetails>("P1", _clientDetails));
		m_sNewestClientName = "P1";
	}

	else if (m_iNumberOfConnectedPlayers == 1) {
		_packet.Serialize(HANDSHAKE, "Server", "P2");
		m_connectedClients.insert(std::pair<std::string, TClientDetails>("P2", _clientDetails));
		m_sNewestClientName = "P2";
	}

	else if (m_iNumberOfConnectedPlayers == 2) {
		_packet.Serialize(HANDSHAKE, "Server", "P3");
		m_connectedClients.insert(std::pair<std::string, TClientDetails>("P3", _clientDetails));
		m_sNewestClientName = "P3";
	}

	else if (m_iNumberOfConnectedPlayers == 3) {
		_packet.Serialize(HANDSHAKE, "Server", "P4");
		m_connectedClients.insert(std::pair<std::string, TClientDetails>("P4", _clientDetails));
		m_sNewestClientName = "P4";
	}

	else {
		return false;
	}

	m_iNumberOfConnectedPlayers++;

	SendDataTo(_packet.PacketData, _clientDetails.m_ClientAddress);

	return true;

}

bool CServer::SendData(char* _pcDataToSend) {
	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	char _RemoteIP[MAX_ADDRESS_LENGTH];
	inet_ntop(AF_INET, &m_ClientAddress.sin_addr, _RemoteIP, sizeof(_RemoteIP));
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
	if (_iBytesToSend != iNumBytes) {
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}

bool CServer::SendDataTo(char* _pcDataToSend, sockaddr_in Address) {

	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	//char _RemoteIP[MAX_ADDRESS_LENGTH];
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

	if (_iBytesToSend != iNumBytes) {
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}
	return true;
}

bool CServer::SendDataToAll(char* _pcDataToSend) {

	int _iBytesToSend;

	int iNumBytes;

	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++) {

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

bool CServer::SendToAllExcept(TClientDetails _client, char* _pcDataToSend) {

	int _iBytesToSend;

	int iNumBytes;

	for (auto it = m_connectedClients.begin(); it != m_connectedClients.end(); it++) {

		if (it->first == _client.m_strName)
			continue;

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

void CServer::ReceiveData(char* _pcBufferToReceiveData) {

	int iSizeOfAdd = sizeof(m_ClientAddress);
	int _iNumOfBytesReceived;
	int _iPacketSize;

	while (m_bOnline) {

		// pull off the packet(s) using recvfrom()
		_iNumOfBytesReceived = recvfrom(
			m_pServerSocket->GetSocketHandle(),						// client-end socket being used to read from
			m_pcPacketData,							// incoming packet to be filled
			MAX_MESSAGE_LENGTH,					   // length of incoming packet to be filled
			0,										// flags
			reinterpret_cast<sockaddr*>(&m_ClientAddress),	// address to be filled with packet source
			&iSizeOfAdd								// size of the above address struct.
			);

		if (_iNumOfBytesReceived < 0) {
			int _iError = WSAGetLastError();
			if (_iError == WSAECONNRESET) {

			}
			else {
				ErrorRoutines::PrintWSAErrorInfo(_iError);
			}
			//return false;
		}

		else if (_iNumOfBytesReceived == 0) {
			//The remote end has shutdown the connection
			_pcBufferToReceiveData = 0;
		}

		else {
			_iPacketSize = strlen(m_pcPacketData) + 1;
			strcpy_s(_pcBufferToReceiveData, _iPacketSize, m_pcPacketData);
			m_pWorkQueue->push(_pcBufferToReceiveData);
		}

		std::this_thread::yield();

	}

}

void CServer::GetRemoteIPAddress(char *_pcSendersIP) {
	char _temp[MAX_ADDRESS_LENGTH];
	int _iAddressLength;
	inet_ntop(AF_INET, &(m_ClientAddress.sin_addr), _temp, sizeof(_temp));
	_iAddressLength = strlen(_temp) + 1;
	strcpy_s(_pcSendersIP, _iAddressLength, _temp);
}

void CServer::ProcessData(char* _pcDataReceived) {

	TPacket _packetRecvd, _packetSent;

	_packetRecvd = _packetRecvd.Deserialize(_pcDataReceived);

	TClientDetails ClientDetails;
	ClientDetails.m_ClientAddress = m_ClientAddress;
	ClientDetails.m_strName = _packetRecvd.Name;

	switch (_packetRecvd.MessageType) {

		case HANDSHAKE:
		{

			char IP[MAX_ADDRESS_LENGTH];
			GetRemoteIPAddress(IP);

			SetColor(YELLOW);
			std::cout << ">> HANDSHAKE request received from " << IP << ":" << GetRemotePort() << std::endl;

			if (!(AddClient(ClientDetails))) {
				SetColor(RED);
				std::cout << "<< HANDSHAKE denied" << std::endl;
				_packetSent.Serialize(HANDSHAKE, "SERVER", "DENIED");
				SendDataTo(_packetSent.PacketData, m_ClientAddress);
			}
			else {

				SetColor(GREEN);
				std::cout << "<< HANDSHAKE successful - User " << m_sNewestClientName << " has joined the room." << std::endl;
			}

			SetColor(GRAY);
			break;

		}
		case DATA:
		{
			_packetSent.Serialize(DATA, _packetRecvd.Name, _packetRecvd.MessageContent);
			SendDataToAll(_packetSent.PacketData);
			break;
		}
		case POSITION:
		{
			_packetRecvd = _packetRecvd.DeserializePosition(_pcDataReceived);

			if (ClientDetails.m_strName == "P1")
				m_pPlayer1->GetModel()->SetPosition(vec3(_packetRecvd.Position.x, _packetRecvd.Position.y, _packetRecvd.Position.z));

			if (ClientDetails.m_strName == "P2")
				m_pPlayer2->GetModel()->SetPosition(vec3(_packetRecvd.Position.x, _packetRecvd.Position.y, _packetRecvd.Position.z));

			if (ClientDetails.m_strName == "P3")
				m_pPlayer3->GetModel()->SetPosition(vec3(_packetRecvd.Position.x, _packetRecvd.Position.y, _packetRecvd.Position.z));

			if (ClientDetails.m_strName == "P4")
				m_pPlayer4->GetModel()->SetPosition(vec3(_packetRecvd.Position.x, _packetRecvd.Position.y, _packetRecvd.Position.z));

			_packetSent.SerializePosition(POSITION, _packetRecvd.Name, _packetRecvd.MessageContent, _packetRecvd.Position);
			SendToAllExcept(ClientDetails, _packetSent.PacketData);

			// Need to move this somewhere else after collisions added
			/*_packetSent.SerializePosition(POSITION, _packetRecvd.Name, _packetRecvd.MessageContent, _packetRecvd.Position);
			SendDataToAll(_packetSent.PacketData);*/


			break;
		}
		case COMMAND:
		{
			char Command = _packetRecvd.MessageContent[2];

			switch (Command) {
				case 'A':
				{
					_packetSent.Serialize(COMMAND, _packetRecvd.Name, "!A");
					SendDataToAll(_packetSent.PacketData);

					m_iActivePlayers++;

					if (m_iActivePlayers == 2) {
						_packetSent.Serialize(COMMAND, _packetRecvd.Name, "!G");
						SendDataToAll(_packetSent.PacketData);
						m_bGameStarted = true;
					}

					break;
				}
				case 'L':
				{

					break;
				}


				default:
				{
					break;
				}
			}

			break;
		}

		case BROADCAST:
		{
			char IP[MAX_ADDRESS_LENGTH];
			GetRemoteIPAddress(IP);

			SetColor(TEAL);
			std::cout << ">> BROADCAST request received from " << IP << ":" << GetRemotePort() << std::endl;
			_packetSent.Serialize(BROADCAST, _packetRecvd.Name, "I'm here!");
			SendData(_packetSent.PacketData);
			SetColor(GRAY);
			break;
		}
		case QUIT:
		{
			SetColor(DPINK);
			std::cout << "Received a quit message from ";
			std::cout << _packetRecvd.Name << std::endl;

			// Remove client from map of clients
			m_connectedClients.erase(_packetRecvd.Name);

			m_iNumberOfConnectedPlayers--;

			std::string Message = "5 Server ";
			Message += _packetRecvd.Name;
			Message += " has left the chat";

			char* CharMessage = &Message[0u];

			SendDataToAll(CharMessage);

			SetColor(GRAY);

			break;
		}
		default:
			break;

	}

}

unsigned short CServer::GetRemotePort() {
	return ntohs(m_ClientAddress.sin_port);
}

void CServer::GetPacketData(char* _pcLocalBuffer) {
	strcpy_s(_pcLocalBuffer, strlen(m_pcPacketData) + 1, m_pcPacketData);
}

CWorkQueue<char*>* CServer::GetWorkQueue() {
	return m_pWorkQueue;
}

std::string CServer::GetServerAddress() {
	std::string s = m_pServerSocket->GetLocalAddress();
	s += ":";
	s += ToString(DEFAULT_SERVER_PORT);
	return s;
}

void CServer::CreateEntities() {

	// Camera

	//	Camera Camera(vec3(0.0f, 15.0f, 22.0f), vec3(0.0f, -0.9f, -1.0f), vec3(0.0f, 1.0f, 0.0f), width, height);   // for map size 15.0f
	Camera Camera(vec3(0.0f, 18.0f, 25.0f), vec3(0.0f, -0.9f, -1.0f), vec3(0.0f, 1.0f, 0.0f), (GLfloat)WIDTH, (GLfloat)HEIGHT);
	//            Pos:  x      y      z    Front:       y tilt          Up:       

	AddCamera(Camera);

	// Player Cubes
	m_pPlayer1 = new Entity;
	m_pPlayer1->SetAsPlayer();
	m_pPlayer1->SetPlayerNumber(P1);
	m_pPlayer1->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(-15.0f, 0.0f, -15.0f), NONENEMY, 8.0f, "Assets//Textures//PlayerTexture.jpg");
	//m_pPlayer1->SetActivity(true);
	AddPlayer(m_pPlayer1);

	m_pPlayer2 = new Entity;
	m_pPlayer2->SetAsPlayer();
	m_pPlayer2->SetPlayerNumber(P2);
	m_pPlayer2->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(15.0f, 0.0f, -15.0f), NONENEMY, 8.0f, "Assets//Textures//PlayerTexture.jpg");
	//m_pPlayer2->SetActivity(true);
	AddPlayer(m_pPlayer2);

	m_pPlayer3 = new Entity;
	m_pPlayer3->SetAsPlayer();
	m_pPlayer3->SetPlayerNumber(P3);
	m_pPlayer3->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(-15.0f, 0.0f, 15.0f), NONENEMY, 8.0f, "Assets//Textures//PlayerTexture.jpg");
	//m_pPlayer3->SetActivity(true);
	AddPlayer(m_pPlayer3);

	m_pPlayer4 = new Entity;
	m_pPlayer4->SetAsPlayer();
	m_pPlayer4->SetPlayerNumber(P4);
	m_pPlayer4->Initialise(PLAYER, CUBE, 36, m_Camera, vec3(15.0f, 0.0f, 15.0f), NONENEMY, 8.0f, "Assets//Textures//PlayerTexture.jpg");
	//m_pPlayer4->SetActivity(true);
	AddPlayer(m_pPlayer4);

}

void CServer::AddPlayer(Entity* _entity) {
	m_players.push_back(_entity);
}

void CServer::AddCamera(Camera _camera) {
	m_Camera = _camera;
}

bool CServer::HasGameStarted() {
	return m_bGameStarted;
}

bool CServer::HasNotifiedPlayers() {
	return m_bNotifiedPlayersOfStart;
}

void CServer::SetNotifiedPlayers(bool _b) {
	m_bNotifiedPlayersOfStart = _b;
}

int CServer::GetNumberOfPlayers() {
	return m_iNumberOfConnectedPlayers;
}
