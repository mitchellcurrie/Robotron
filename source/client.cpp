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
#include "TextLabel.h"
#include "server.h"

//This includes
#include "client.h"

CClient::CClient()
	:m_pcPacketData(0)
	, m_pClientSocket(0)
	, m_bGameStarted(false) {
	ZeroMemory(&m_ServerSocketAddress, sizeof(m_ServerSocketAddress));

	//Create a Packet Array and fill it out with all zeros.
	m_pcPacketData = new char[MAX_MESSAGE_LENGTH];
	ZeroMemory(m_pcPacketData, MAX_MESSAGE_LENGTH);
	m_bQuit, m_bHasCollided = false;
}

CClient::~CClient() {
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
bool CClient::Initialise() {

	/*m_pPlayer1 = new Player;
	m_pPlayer1->SetName("P1");
	m_pPlayer1->SetActivity(false);

	m_pPlayer2 = new Player;
	m_pPlayer2->SetName("P2");
	m_pPlayer2->SetActivity(false);

	m_pPlayer3 = new Player;
	m_pPlayer3->SetName("P3");
	m_pPlayer3->SetActivity(false);

	m_pPlayer4 = new Player;
	m_pPlayer4->SetName("P4");
	m_pPlayer4->SetActivity(false);*/


	//Local Variables to hold Server's IP address and Port NUmber as entered by the user
	char _cServerIPAddress[128];
	ZeroMemory(&_cServerIPAddress, 128);
	char _cServerPort[10];
	ZeroMemory(&_cServerPort, 10);
	//unsigned short _usServerPort;

	char _cServerChosen[5];
	ZeroMemory(&_cServerChosen, 5);
	//unsigned int _uiServerIndex;

	//Local variable to hold client's name
	//char _cUserName[50];
	ZeroMemory(&m_cUserName, 50);

	//Zero out the memory for all the member variables.
	ZeroMemory(&m_cUserName, strlen(m_cUserName));


	//Create a work queue to distribute messages between the main  thread and the receive thread.
	m_pWorkQueue = new CWorkQueue<char*>();

	//Create a socket object
	m_pClientSocket = new CSocket();

	//Get the port number to bind the socket to
	//Initialise the socket to the local loop back address and port number
	if (!m_pClientSocket->Initialise("127.0.0.1", DEFAULT_CLIENT_PORT)) {
		return false;
	}

	//Set the client's online status to true
	m_bOnline = true;

	m_bDoBroadcast = true;
	m_pClientSocket->EnableBroadcast();
	BroadcastForServers();

	if (m_vecServerAddr.size() > 0) {

		//Give a list of servers for the user to choose from :
		for (unsigned int i = 0; i < m_vecServerAddr.size(); i++) {
			inet_ntop(AF_INET, &(m_vecServerAddr[i].sin_addr), _cServerIPAddress, MAX_ADDRESS_LENGTH);
			m_cIPAddresses.push_back(_cServerIPAddress);
		}

	}

	m_pPlayer1 = new Player;
	m_pPlayer1->SetName("P1");
	m_pPlayer1->SetActivity(false);

	m_pPlayer2 = new Player;
	m_pPlayer2->SetName("P2");
	m_pPlayer2->SetActivity(false);

	m_pPlayer3 = new Player;
	m_pPlayer3->SetName("P3");
	m_pPlayer3->SetActivity(false);

	m_pPlayer4 = new Player;
	m_pPlayer4->SetName("P4");
	m_pPlayer4->SetActivity(false);

	for (int x{ 0 }; x < 8; x++) {

		m_pEnemy = new Enemy;
		AddEnemy(m_pEnemy);
		m_pEnemy->SetActivity(false);

	}

	m_pPowerUp = new PowerUp;
	m_pPowerUp->SetActivity(false);


	for (int x{ 0 }; x < 10; x++) {

		m_pBullet = new Bullet;
		m_pBullet->SetActivity(false);
		m_player1Bullets.push_back(m_pBullet);
	}


	for (int x{ 0 }; x < 10; x++) {

		m_pBullet = new Bullet;
		m_pBullet->SetActivity(false);
		m_player2Bullets.push_back(m_pBullet);
	}


	for (int x{ 0 }; x < 10; x++) {

		m_pBullet = new Bullet;
		m_pBullet->SetActivity(false);
		m_player3Bullets.push_back(m_pBullet);
	}


	for (int x{ 0 }; x < 10; x++) {

		m_pBullet = new Bullet;
		m_pBullet->SetActivity(false);
		m_player4Bullets.push_back(m_pBullet);
	}

	return true;

}

bool CClient::SendData(char* _pcDataToSend) {

	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	char _RemoteIP[MAX_ADDRESS_LENGTH];
	inet_ntop(AF_INET, &m_ServerSocketAddress.sin_addr, _RemoteIP, sizeof(_RemoteIP));
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

	if (_iBytesToSend != iNumBytes) {
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}

	return true;

}

bool CClient::SendDataTo(char* _pcDataToSend, sockaddr_in Address) {

	int _iBytesToSend = (int)strlen(_pcDataToSend) + 1;

	char _RemoteIP[MAX_ADDRESS_LENGTH];
	inet_ntop(AF_INET, &Address.sin_addr, _RemoteIP, sizeof(_RemoteIP));
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

	if (_iBytesToSend != iNumBytes) {
		std::cout << "There was an error in sending data from client to server" << std::endl;
		return false;
	}

	return true;

}

void CClient::ReceiveBroadcastMessages(char* _pcBufferToReceiveData) {

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

	while (m_bDoBroadcast) {

		// pull off the packet(s) using recvfrom()
		int _iNumOfBytesReceived = recvfrom(
			this->m_pClientSocket->GetSocketHandle(),	// client-end socket being used to read from
			_buffer,									// incoming packet to be filled
			MAX_MESSAGE_LENGTH,							// length of incoming packet to be filled
			0,											// flags
			reinterpret_cast<sockaddr*>(&_FromAddress),	// address to be filled with packet source
			&iSizeOfAdd								// size of the above address struct.
			);

		if (_iNumOfBytesReceived < 0) {
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
		else if (_iNumOfBytesReceived == 0) {
			//The remote end has shutdown the connection
			_pcBufferToReceiveData = 0;
		}
		else {
			//There is valid data received.
			strcpy_s(_pcBufferToReceiveData, strlen(_buffer) + 1, _buffer);
			m_ServerSocketAddress = _FromAddress;
			m_vecServerAddr.push_back(m_ServerSocketAddress);
		}

	}//End of while loop

}

bool CClient::BroadcastForServers() {

	//Make a broadcast packet
	TPacket _packet;
	_packet.SerializeMessage(BROADCAST, m_cUserName, "DetectServer");
	char _temp[MAX_MESSAGE_LENGTH];

	//Send out a broadcast message using the broadcast address
	m_pClientSocket->SetRemoteAddress(INADDR_BROADCAST);
	m_pClientSocket->SetRemotePort(DEFAULT_SERVER_PORT);

	m_ServerSocketAddress.sin_family = AF_INET;
	m_ServerSocketAddress.sin_addr.S_un.S_addr = INADDR_BROADCAST;

	for (int i = 0; i < 1000; i++) //Just try  a series of 10 ports to detect a runmning server; this is needed since we are testing multiple servers on the same local machine
	{
		m_ServerSocketAddress.sin_port = htons(DEFAULT_SERVER_PORT + i);
		SendData(_packet.PacketData);
	}

	ReceiveBroadcastMessages(_temp);

	return true;
}

void CClient::ReceiveData(char* _pcBufferToReceiveData) {

	struct timeval timeValue;
	timeValue.tv_sec = 0;
	timeValue.tv_usec = 0;
	setsockopt(m_pClientSocket->GetSocketHandle(), SOL_SOCKET, SO_RCVTIMEO, (char*)&timeValue, sizeof(timeValue));

	sockaddr_in _FromAddress; // Make a local variable to extract the IP and port number of the sender from whom we are receiving
							  //In this case; it should be the details of the server; since the client only ever receives from the server

	int iSizeOfAdd = sizeof(_FromAddress);
	int _iNumOfBytesReceived;

	//Receive data into a local buffer
	char _buffer[MAX_MESSAGE_LENGTH];

	//For debugging purpose only, convert the Address structure to a string.
	char _pcAddress[50];
	ZeroMemory(&_pcAddress, 50);

	while (m_bOnline) {

		// pull off the packet(s) using recvfrom()
		_iNumOfBytesReceived = recvfrom(
			this->m_pClientSocket->GetSocketHandle(),						// client-end socket being used to read from
			_buffer,							// incoming packet to be filled
			MAX_MESSAGE_LENGTH,					   // length of incoming packet to be filled
			0,										// flags
			reinterpret_cast<sockaddr*>(&_FromAddress),	// address to be filled with packet source
			&iSizeOfAdd								// size of the above address struct.
			);

		inet_ntop(AF_INET, &_FromAddress, _pcAddress, sizeof(_pcAddress));

		if (_iNumOfBytesReceived < 0) {
			int _iError = WSAGetLastError();

			if (_iError == WSAECONNRESET) {

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);

				TPacket _packet;
				_packet.SerializeMessage(DATA, "Error", "Server dropped. Please close the window.");

				m_pWorkQueue->push(_packet.PacketData);

			}
			else {
				//Error in receiving data 
				std::cout << "recvfrom failed with error " << WSAGetLastError();
			}

			//_pcBufferToReceiveData = 0;
			m_pcPacketData = 0;
		}
		else if (_iNumOfBytesReceived == 0) {
			//The remote end has shutdown the connection
			// _pcBufferToReceiveData = 0;
			m_pcPacketData = 0;
		}
		else {

			m_ServerSocketAddress = _FromAddress;
			strcpy_s(m_pcPacketData, strlen(_buffer) + 1, _buffer);
			m_pWorkQueue->push(m_pcPacketData);

		}

		std::this_thread::yield();

	}

}

void CClient::ProcessData(char* _pcDataReceived) {

	TPacket _packetRecvd = _packetRecvd.DeserializeMessage(_pcDataReceived);
	TPacket _packetSent;

	switch (_packetRecvd.MessageType) {

		case HANDSHAKE:
		{
			std::string Content = _strdup(_packetRecvd.MessageContent);
			SetColor(YELLOW);

			if (Content == " DENIED") {
				std::cout << "Already 4 Players!" << std::endl;

				while (true) {

				}

			}
			else {
				strcpy_s(m_cUserName, strlen(_packetRecvd.MessageContent) + 1, _packetRecvd.MessageContent);
				std::cout << "YOU'VE JOINED THE SERVER" << std::endl;

				std::string UserName = m_cUserName;

				if (UserName == "P1")
					m_pPlayer1->SetActivity(true);
				else if (UserName == "P2")
					m_pPlayer2->SetActivity(true);
				else if (UserName == "P3")
					m_pPlayer3->SetActivity(true);
				else if (UserName == "P4")
					m_pPlayer4->SetActivity(true);


				SetColor(GRAY);
				break;
			}
		}
		case DATA:
		{
			SetColor(DGREEN);
			std::cout << "DATA - " << _packetRecvd.Name << ": " << _packetRecvd.MessageContent << std::endl;
			SetColor(GRAY);
			break;
		}
		case POSITION_P:
		{

			_packetSent = _packetRecvd.DeserializePlayerPositionsWithActiveState(_pcDataReceived);

			if (_packetSent.Players[0].bIsActive) {
				m_pPlayer1->SetActivity(true);
			}
			else if (_packetSent.Players[1].bIsActive) {
				m_pPlayer2->SetActivity(true);
			}
			else if (_packetSent.Players[2].bIsActive) {
				m_pPlayer3->SetActivity(true);
			}
			else if (_packetSent.Players[3].bIsActive) {
				m_pPlayer4->SetActivity(true);
			}

			m_pPlayer1->SetPosition(glm::vec3(_packetSent.Players[0].position));
			m_pPlayer2->SetPosition(glm::vec3(_packetSent.Players[1].position));
			m_pPlayer3->SetPosition(glm::vec3(_packetSent.Players[2].position));
			m_pPlayer4->SetPosition(glm::vec3(_packetSent.Players[3].position));

			break;
		}
		case POSITION_B:
		{
			_packetSent = _packetRecvd.DeserializeBullets(_pcDataReceived);
			std::string strName = _packetSent.Name;

			if (strName == "P1") {

				for (int i = 0; i < 10; i++) {

					m_player1Bullets[i]->SetPosition(glm::vec3(_packetSent.Bullets[i].position));
					m_player1Bullets[i]->SetActivity(_packetSent.Bullets[i].bIsActive);
				}
			}

			if (strName == "P2") {

				for (int i = 0; i < 10; i++) {

					m_player2Bullets[i]->SetPosition(glm::vec3(_packetSent.Bullets[i].position));
					m_player2Bullets[i]->SetActivity(_packetSent.Bullets[i].bIsActive);
				}
			}

			if (strName == "P3") {

				for (int i = 0; i < 10; i++) {

					m_player3Bullets[i]->SetPosition(glm::vec3(_packetSent.Bullets[i].position));
					m_player3Bullets[i]->SetActivity(_packetSent.Bullets[i].bIsActive);
				}
			}

			if (strName == "P4") {

				for (int i = 0; i < 10; i++) {

					m_player4Bullets[i]->SetPosition(glm::vec3(_packetSent.Bullets[i].position));
					m_player4Bullets[i]->SetActivity(_packetSent.Bullets[i].bIsActive);
				}
			}

			break;
		}
		case POSITION_E:
		{
			_packetSent = _packetRecvd.DeserializeEnemies(_pcDataReceived);

			for (int i = 0; i < 8; i++) {

				m_enemies[i]->SetPosition(glm::vec3(_packetSent.Enemies[i].position));
				m_enemies[i]->SetActivity(_packetSent.Enemies[i].bIsActive);

			}

			break;
		}
		case POSITION_PU:
		{
			_packetSent = _packetRecvd.DeserializePowerUp(_pcDataReceived);


			m_pPowerUp->SetPosition(glm::vec3(_packetSent.PowerUp1.position.x, 0.0f, _packetSent.PowerUp1.position.z));
			m_pPowerUp->SetActivity(_packetSent.PowerUp1.bIsActive);

			break;
		}
		case BROADCAST:
		{
			m_vecServerAddr.push_back(m_ServerSocketAddress);
			break;
		}
		case KEEPALIVE:
		{

			break;
		}
		case COMMAND:
		{
			char Command = _packetRecvd.MessageContent[2];
			SetColor(PINK);
			std::cout << "COMMAND ";

			switch (Command) {
				case 'A':
				{
					std::cout << _packetRecvd.Name << " is active" << std::endl;
					m_bIsActive = true;
					break;
				}
				case 'G':
				{
					m_bGameStarted = true;
					break;
				}

				case 'C':
				{
					m_bHasCollided = true;
					break;
				}

				case 'N':
				{
					m_bHasCollided = false;
					break;
				}

				case 'P':
				{

					_packetSent.SerializeMessage(COMMAND, "SERVER", "P");
					SendData(_packetSent.PacketData);

					break;
				}

				default:
				{
					break;
				}

			}

			SetColor(GRAY);
			break;
		}
		case QUIT:
		{
			SetColor(RED);
			std::cout << "QUIT - " << _packetRecvd.Name << ":" << _packetRecvd.MessageContent << std::endl;
			SetColor(GRAY);
			break;
		}
		default:
			break;

	}

}

bool CClient::IsActive() {

	return false;
}

bool CClient::IsActive(std::string _player) {

	if (_player == "P1") {
		if (m_pPlayer1->IsActive())
			return true;
	}
	else if (_player == "P2") {
		if (m_pPlayer2->IsActive())
			return true;
	}
	else if (_player == "P3") {
		if (m_pPlayer3->IsActive())
			return true;
	}
	else if (_player == "P4") {
		if (m_pPlayer4->IsActive())
			return true;
	}

	return false;
}

glm::vec3 CClient::GetPosition(std::string _playerName) {

	if (_playerName == "P1")
		return m_pPlayer1->GetPosition();

	else if (_playerName == "P2")
		return m_pPlayer2->GetPosition();

	else if (_playerName == "P3")
		return m_pPlayer3->GetPosition();

	else if (_playerName == "P4")
		return m_pPlayer4->GetPosition();

	return (glm::vec3(0));

}

void CClient::ConnectToServer(int _iIndex) {

	char _cServerIPAddress[128];
	ZeroMemory(&_cServerIPAddress, 128);
	strcpy_s(_cServerIPAddress, m_cIPAddresses.at(_iIndex).c_str());

	m_ServerSocketAddress.sin_family = AF_INET;
	m_ServerSocketAddress.sin_port = m_vecServerAddr[_iIndex].sin_port;
	m_ServerSocketAddress.sin_addr.S_un.S_addr = m_vecServerAddr[_iIndex].sin_addr.S_un.S_addr;
	inet_ntop(AF_INET, &(m_vecServerAddr[_iIndex].sin_addr), _cServerIPAddress, MAX_ADDRESS_LENGTH);

	m_bDoBroadcast = false;
	m_pClientSocket->DisableBroadcast();

	TPacket _packet;
	_packet.SerializeMessage(HANDSHAKE, m_cUserName, m_cUserName);
	SendData(_packet.PacketData);

}

void CClient::GetRemoteIPAddress(char *_pcSendersIP) {
	inet_ntop(AF_INET, &(m_ServerSocketAddress.sin_addr), _pcSendersIP, sizeof(_pcSendersIP));
	return;
}

unsigned short CClient::GetRemotePort() {
	return ntohs(m_ServerSocketAddress.sin_port);
}

void CClient::GetPacketData(char* _pcLocalBuffer) {
	strcpy_s(_pcLocalBuffer, strlen(m_pcPacketData) + 1, m_pcPacketData);
}

CWorkQueue<char*>* CClient::GetWorkQueue() {
	return m_pWorkQueue;
}

char* CClient::GetName() {
	return m_cUserName;
}

std::vector<sockaddr_in> CClient::GetPorts() {
	return m_vecServerAddr;
}

std::vector<std::string> CClient::GetIPAddresses() {
	return m_cIPAddresses;
}

bool CClient::HasGameStarted() {
	return m_bGameStarted;
}

void CClient::SetCollision(bool _b) {
	m_bHasCollided = _b;
}

bool CClient::HasCollided() {
	return m_bHasCollided;
}

void CClient::AddEnemy(Enemy* _enemy) {
	m_enemies.push_back(_enemy);
}

std::vector<Enemy*> CClient::GetEnemies() {

	return m_enemies;
}

std::vector<Bullet*> CClient::GetP1Bullets(){
	return m_player1Bullets;
}

std::vector<Bullet*> CClient::GetP2Bullets() {
	return m_player2Bullets;
}

std::vector<Bullet*> CClient::GetP3Bullets() {
	return m_player3Bullets;
}

std::vector<Bullet*> CClient::GetP4Bullets() {
	return m_player4Bullets;
}

PowerUp * CClient::GetPowerUp() {
	return m_pPowerUp;
}