//
// Bachelor of Software Engineering
// Media Design School
// Auckland
// New Zealand
//
// (c) 2015 Media Design School
//
// File Name	: networkentity.h
// Description	: header file
// Author		: Mitchell Currie
// Mail			: Mitchell.Currie@mediadesign.school.nz

#pragma once

#ifndef __NETWORKENTITY_H__
#define __NETWORKENTITY_H__

#include <string>
#include <sstream>

#include <ft2build.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


enum EMessageType : unsigned short
{
	HANDSHAKE,
	DATA,
	POSITION,
	KEEPALIVE,
	COMMAND,
	BROADCAST,
	QUIT
};

struct TPacket 
{
	unsigned short MessageType;
	char MessageContent[256];
	char Name[50];
	char PacketData[60];
	unsigned short PacketSize;
	glm::vec3 Position;
	unsigned char Key1;  // for bullets
	unsigned char Key2;  // for bullets

	//void set_packet(short _x, short _y, WORD _object_type, short _object_index, WORD _param)
	void Serialize(EMessageType _MessageType, char* _name, char* _message)
	{
		
		MessageType = _MessageType;
		strcpy_s(MessageContent, strlen(_message) + 1, _message);
		strcpy_s(Name, strlen(_name) + 1, _name);

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << Name;
		oss << " ";
		oss << MessageContent;
		
		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = _strToSend.size();
	}
	
	TPacket Deserialize(char* _PacketData)
	{
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		iss >> this->MessageType;
		iss >> this->Name;
		std::string _tempMessageBuffer;

		getline(iss, _tempMessageBuffer);
		strcpy_s(MessageContent, _tempMessageBuffer.length() + 1, _tempMessageBuffer.c_str());

		return *this;
	}

	void SerializePosition(EMessageType _MessageType, char* _name, char* _message, glm::vec3 _position, unsigned char _Key1, unsigned char _Key2)
	{

		MessageType = _MessageType;
		strcpy_s(MessageContent, strlen(_message) + 1, _message);
		strcpy_s(Name, strlen(_name) + 1, _name);
		Position.x = _position.x;
		Position.y = _position.y;
		Position.z = _position.z;
		Key1 = _Key1;
		Key2 = _Key2;

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << Name;
		oss << " ";
		oss << MessageContent;
		oss << " ";
		oss << Position.x;   // Clients player position
		oss << " ";
		oss << Position.y;
		oss << " ";
		oss << Position.z;
		oss << " ";
		oss << Key1;    // Bullet key presses
		oss << " ";
		oss << Key2;

		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = _strToSend.size();
	}

	TPacket DeserializePositionWithKeys(char* _PacketData)
	{
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		iss >> this->MessageType;
		iss >> this->Name;
		iss >> this->MessageContent;
		iss >> this->Position.x;   // Clients player position
		iss >> this->Position.y;
		iss >> this->Position.z;
		iss >> this->Key1;		 // Bullet key presses
		iss >> this->Key2;

		/*std::string _tempMessageBuffer;

		getline(iss, _tempMessageBuffer);
		strcpy_s(MessageContent, _tempMessageBuffer.length() + 1, _tempMessageBuffer.c_str());*/

		return *this;
	}

	void SerializePosition(EMessageType _MessageType, char* _name, char* _message, glm::vec3 _position)
	{

		MessageType = _MessageType;
		strcpy_s(MessageContent, strlen(_message) + 1, _message);
		strcpy_s(Name, strlen(_name) + 1, _name);
		Position.x = _position.x;
		Position.y = _position.y;
		Position.z = _position.z;

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << Name;
		oss << " ";
		oss << MessageContent;
		oss << " ";
		oss << Position.x;   // Clients player position
		oss << " ";
		oss << Position.y;
		oss << " ";
		oss << Position.z;

		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = _strToSend.size();
	}

	TPacket DeserializePosition(char* _PacketData)
	{
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		iss >> this->MessageType;
		iss >> this->Name;
		iss >> this->MessageContent;
		iss >> this->Position.x;
		iss >> this->Position.y;
		iss >> this->Position.z; // Clients player position

		/*std::string _tempMessageBuffer;

		getline(iss, _tempMessageBuffer);
		strcpy_s(MessageContent, _tempMessageBuffer.length() + 1, _tempMessageBuffer.c_str());*/

		return *this;
	}
};

class INetworkEntity
{
public:
	virtual bool Initialise() = 0; //Implicit in the intialization is the creation and binding of the socket
	virtual bool SendData(char* _pcDataToSend) = 0;
	virtual void ReceiveData(char* _pcBufferToReceiveData) = 0;
	virtual void GetRemoteIPAddress(char *_pcSendersIP) = 0;
	virtual unsigned short GetRemotePort() = 0;

protected:
	bool m_bOnline;
};
#endif 