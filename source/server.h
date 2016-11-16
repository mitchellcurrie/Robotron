//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: server.h
// Description	: header file
// Author		: Mitchell Currie
// Mail			: Mitchell.Currie@mediadesign.school.nz

#pragma once

#ifndef __SERVER_H__
#define __SERVER_H__

// Library Includes
#include <Windows.h>
#include <map>
#include <time.h>
#include <ctime>
#include <vector>

// Local Includes
#include "networkentity.h"
#include "WorkQueue.h"
#include "entity.h"

// Types

// Constants

//Forward Declaration
class CSocket;

//Structure to hold the details of all connected clients
struct TClientDetails
{
	sockaddr_in m_ClientAddress;
	//bool m_bIsActive;
	std::string m_strName;
	//time_t m_timeOfLastMessage;
};

class CServer : public INetworkEntity
{
public:
	// Default Constructors/Destructors
	CServer();
	~CServer();

	// Virtual Methods from the Network Entity Interface.
	virtual bool Initialise(); //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* _pcDataToSend);
	virtual void ReceiveData(char* _pcBufferToReceiveData);
	virtual void GetRemoteIPAddress(char* _pcSendersIP);
	virtual unsigned short GetRemotePort();
	virtual void ProcessData(char* _pcDataReceived);

	virtual bool SendDataTo(char* _pcDataToSend, sockaddr_in Addreses);
	virtual bool SendDataToAll(char* _pcDataToSend);
	bool SendToAllExcept(TClientDetails _client, char* _pcDataToSend);
	bool HasGameStarted();
	bool HasNotifiedPlayers();
	void SetNotifiedPlayers(bool _b);
	int GetNumberOfPlayers();

	void GetPacketData(char* _pcLocalBuffer);
	CWorkQueue<char*>* GetWorkQueue();

	void CreateEntities();
	void AddPlayer(Player* _player);
	Player* GetPlayer(std::string _player);
	void SendPositionsToPlayers();

	std::string GetServerAddress();

	//Qs 2: Function to add clients to the map.
private:
	bool AddClient(TClientDetails _clientDetail);
	
private:
	//A Buffer to contain all packet data for the server
	char* m_pcPacketData;
	//A server has a socket object to create the UDP socket at its end.
	CSocket* m_pServerSocket;
	// Make a member variable to extract the IP and port number of the sender from whom we are receiving
	//Since it is a UDP socket capable of receiving from multiple clients; these details will change depending on who has sent the packet we are currently processing.
	sockaddr_in m_ClientAddress; 

	//Qs 2 : Make a map to hold the details of all the client who have connected. 
	//The structure maps client name to their addresses
	std::map<std::string, TClientDetails> m_connectedClients;

	
	//A workQueue to distribute messages between the main thread and Receive thread.
	CWorkQueue<char*>* m_pWorkQueue;

	static std::string m_sNewestClientName;

	/////////////////////////////  Timer start   //////////////////////////////////////////
	static std::clock_t m_start;
	static double m_duration;

	std::vector<std::string> m_VecNames;
	std::vector<char*> m_Quitters;

	static int m_iNumberOfConnectedPlayers;

	bool m_bGameStarted;
	bool m_bNotifiedPlayersOfStart;

	int m_iActivePlayers;

	//// Players
	std::vector<Player*> m_players;
	Player* m_pPlayer1;
	Player* m_pPlayer2;
	Player* m_pPlayer3;
	Player* m_pPlayer4;
};

#endif