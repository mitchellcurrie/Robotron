//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: socket.h
// Description	: header file
// Author		: Mitchell Currie
// Mail			: Mitchell.Currie@mediadesign.school.nz

#pragma once

#ifndef __SOCKET_H__
#define __SOCKET_H__

// Library Includes
#include <windows.h>
#include <string>


class CSocket
{
public:
	// Default Constructors/Destructors
	CSocket();
	~CSocket();

	//This function call does the act of creating a socket and binding it to the supplied port number on the local machine.
	bool Initialise(char* _pcIPAddress, unsigned short _usPortNumber);

	//Accessor methods
	SOCKET GetSocketHandle();

	//Additions 
	void SetRemotePort(unsigned short _usRemotePort);
	void SetRemoteAddress(unsigned long _ulRemoteAddress);
	std::string GetLocalAddress();

	// Broadcasting
	int EnableBroadcast();
	int DisableBroadcast();

private:
	//A handle to as of yet unbound socket
	SOCKET m_hSocket;
	//A sockaddr structure containing the local address and port to bind the socket to.
	sockaddr_in m_SocketAddress;

	//Addition : Details of the remote machine which is sending/receving data via this socket object.
	unsigned long m_ulRemoteIPAddress;
	unsigned short m_usRemotePort;
	
};

#endif