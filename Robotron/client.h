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
	void Process(char* BufferToReceive);

	void GetPacketData(char* _pcLocalBuffer);
	CWorkQueue<char*>* GetWorkQueue();

	bool BroadcastForServers();

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
	bool m_bDoBroadcast;

	bool m_bGameStarted;
	

};

#endif