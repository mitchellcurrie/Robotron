//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: client.h
// Description	: client header file
// Author		: Mitchell Currie
// Mail			: Mitchell.Currie@mediadesign.school.nz
//
#pragma once

#ifndef __CLIENT_H__
#define __CLIENT_H__

// Library Includes

// Local Includes
#include "networkentity.h"
#include "WorkQueue.h"
#include "entity.h"

// Types

// Constants

//////////////////////////////////////
/*
	All this stuff in each entity

*/

//////////////////////////////////////


//Forward Declaration
class CSocket;

class CClient : public INetworkEntity
{
public:
	// Default Constructors/Destructors
	CClient();
	~CClient();

	// Methods
	
	virtual bool Initialise(); //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* _pcDataToSend);
	virtual bool SendDataTo(char* _pcDataToSend, sockaddr_in Address);
	virtual void ReceiveData(char* _pcBufferToReceiveData);
	virtual void GetRemoteIPAddress(char* _pcSendersIP);
	virtual unsigned short GetRemotePort();
	bool HasGameStarted();
	char* GetName();
	void ProcessData(char* BufferToReceive);

	void GetPacketData(char* _pcLocalBuffer);
	CWorkQueue<char*>* GetWorkQueue();

	bool BroadcastForServers();

	std::vector<sockaddr_in> GetPorts();
	std::vector<std::string> GetIPAddresses();

	void ConnectToServer(int _iIndex);

	// GameScene
	bool IsActive();
	bool HasGameStarted() const;
	bool IsActive(std::string _player);
	glm::vec3 GetPosition(std::string _playerName);
	std::vector<Enemy*> GetEnemies();

	void SetCollision(bool _b);
	bool HasCollided();

	void AddEnemy(Enemy* _enemy);


private:
	// Question 7 : Broadcast to Detect Servers
	void ReceiveBroadcastMessages(char* _pcBufferToReceiveData);

private:
	//A buffer to contain all packet data for the client
	char* m_pcPacketData;
	//A client has a socket object to create the UDP socket at its end.
	CSocket* m_pClientSocket;
	// A Sockaddress structure which will have the details of the server 
	sockaddr_in m_ServerSocketAddress;
	//A username to associate with a client
	char m_cUserName[50];
	//A workQueue to distribute messages between the main thread and Receive thread.
	CWorkQueue<char*>* m_pWorkQueue;

	bool m_bQuit;

	//Question 7
	//A vector to hold all the servers found after broadcasting
	std::vector<sockaddr_in> m_vecServerAddr;
	std::vector<std::string> m_cIPAddresses;
	bool m_bDoBroadcast;

	bool m_bGameStarted;
	bool m_bHasCollided;

	// GameScene
	bool m_bIsActive;

	// Players
	std::vector<Player*> m_playerPositions;
	Player* m_pPlayer1;
	Player* m_pPlayer2;
	Player* m_pPlayer3;
	Player* m_pPlayer4;

	std::vector<Enemy*> m_enemies;
	Enemy* m_pEnemy;

};

#endif