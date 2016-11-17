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
#include <vector>
#include "utils.h"

#include <ft2build.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


enum EMessageType : unsigned short {
	HANDSHAKE,
	DATA,
	POSITION_P,
	POSITION_E,
	POSITION_PU,
	POSITION_B,
	KEEPALIVE,
	COMMAND,
	BROADCAST,
	QUIT
};

struct TPacket {
	unsigned short MessageType;
	char MessageContent[60];
	char Name[50];
	char PacketData[256];
	unsigned short PacketSize;

	glm::vec3 Position;
	glm::vec3 PlayerPositions[4];
	std::vector<Player> Players;
	std::vector<Enemy> Enemies;
	std::vector<Bullet> Bullets;
	PowerUp PowerUp1;
	bool bIsActive;

	//void set_packet(short _x, short _y, WORD _object_type, short _object_index, WORD _param)
	void SerializeMessage(EMessageType _MessageType, char* _name, char* _message) {

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

		PacketSize = (unsigned short)_strToSend.size();
	}

	TPacket DeserializeMessage(char* _PacketData) {
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		iss >> this->MessageType;
		iss >> this->Name;
		std::string _tempMessageBuffer;

		getline(iss, _tempMessageBuffer);
		strcpy_s(MessageContent, _tempMessageBuffer.length() + 1, _tempMessageBuffer.c_str());

		return *this;
	}

	void SerializePosition(EMessageType _MessageType, char* _name, char* _message, glm::vec3 _position) {

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
		oss << Position.x;
		oss << " ";
		oss << Position.z;

		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = (unsigned short)_strToSend.size();
	}

	TPacket DeserializePosition(char* _PacketData) {
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		iss >> this->MessageType;
		iss >> this->Name;
		iss >> this->MessageContent;
		iss >> this->Position.x;
		iss >> this->Position.z;

		return *this;
	}

	void SerializePlayerPositions(EMessageType _MessageType, glm::vec3 _position1, glm::vec3 _position2, glm::vec3 _position3, glm::vec3 _position4) {

		MessageType = _MessageType;

		PlayerPositions[0] = _position1;
		PlayerPositions[1] = _position2;
		PlayerPositions[2] = _position3;
		PlayerPositions[3] = _position4;

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << PlayerPositions[0].x;
		oss << " ";
		oss << PlayerPositions[0].z;
		oss << " ";
		oss << PlayerPositions[1].x;
		oss << " ";
		oss << PlayerPositions[1].z;
		oss << " ";
		oss << PlayerPositions[2].x;
		oss << " ";
		oss << PlayerPositions[2].z;
		oss << " ";
		oss << PlayerPositions[3].x;
		oss << " ";
		oss << PlayerPositions[3].z;

		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = (unsigned short)_strToSend.size();
	
	}

	
	TPacket DeserializePlayerPositions(char* _PacketData) {
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		iss >> this->MessageType;
		iss >> this->PlayerPositions[0].x;
		iss >> this->PlayerPositions[0].z;
		iss >> this->PlayerPositions[1].x;
		iss >> this->PlayerPositions[1].z;
		iss >> this->PlayerPositions[2].x;
		iss >> this->PlayerPositions[2].z;
		iss >> this->PlayerPositions[3].x;
		iss >> this->PlayerPositions[3].z;

		return *this;
	}



	void SerializePlayerPositions(EMessageType _MessageType, Player _p1, Player _p2, Player _p3, Player _p4) {

		MessageType = _MessageType;

		Players.push_back(_p1);
		Players.push_back(_p2);
		Players.push_back(_p3);
		Players.push_back(_p4);

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << Players[0].position.x;
		oss << " ";
		oss << Players[0].position.z;
		oss << " ";
		oss << Players[0].bIsActive;
		oss << " ";
		oss << Players[1].position.x;
		oss << " ";
		oss << Players[1].position.z;
		oss << " ";
		oss << Players[1].bIsActive;
		oss << " ";
		oss << Players[2].position.x;
		oss << " ";
		oss << Players[2].position.z;
		oss << " ";
		oss << Players[2].bIsActive;
		oss << " ";
		oss << Players[3].position.x;
		oss << " ";
		oss << Players[3].position.z;
		oss << " ";
		oss << Players[3].bIsActive;

		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = (unsigned short)_strToSend.size();

	}

	TPacket DeserializePlayerPositionsWithActiveState(char* _PacketData) {
	
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		Players.clear();

		Players.reserve(4);

		for (int i = 0; i < 4; i++) {
			Player temp;
			Players.push_back(temp);
		}

		iss >> this->MessageType;
		
		for (int i = 0; i < 4; i++) {

			iss >> this->Players[i].position.x;
			iss >> this->Players[i].position.z;
			iss >> this->Players[i].bIsActive;

		}

		return *this;
	}

	void SerializeEnemies(EMessageType _MessageType, Enemy _enemy1, Enemy _enemy2, Enemy _enemy3, Enemy _enemy4, Enemy _enemy5, Enemy _enemy6, Enemy _enemy7, Enemy _enemy8) {

		MessageType = _MessageType;

		Enemies.clear();

		Enemies.push_back(_enemy1);
		Enemies.push_back(_enemy2);
		Enemies.push_back(_enemy3);
		Enemies.push_back(_enemy4);
		Enemies.push_back(_enemy5);
		Enemies.push_back(_enemy6);
		Enemies.push_back(_enemy7);
		Enemies.push_back(_enemy8);

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << Enemies[0].position.x;
		oss << " ";
		oss << Enemies[0].position.z;
		oss << " ";
		oss << Enemies[0].bIsActive;
		oss << " ";
		oss << Enemies[1].position.x;
		oss << " ";
		oss << Enemies[1].position.z;
		oss << " ";
		oss << Enemies[1].bIsActive;
		oss << " ";
		oss << Enemies[2].position.x;
		oss << " ";
		oss << Enemies[2].position.z;
		oss << " ";
		oss << Enemies[2].bIsActive;
		oss << " ";
		oss << Enemies[3].position.x;
		oss << " ";
		oss << Enemies[3].position.z;
		oss << " ";
		oss << Enemies[3].bIsActive;
		oss << " ";
		oss << Enemies[4].position.x;
		oss << " ";
		oss << Enemies[4].position.z;
		oss << " ";
		oss << Enemies[4].bIsActive;
		oss << " ";
		oss << Enemies[5].position.x;
		oss << " ";
		oss << Enemies[5].position.z;
		oss << " ";
		oss << Enemies[5].bIsActive;
		oss << " ";
		oss << Enemies[6].position.x;
		oss << " ";
		oss << Enemies[6].position.z;
		oss << " ";
		oss << Enemies[6].bIsActive;
		oss << " ";
		oss << Enemies[7].position.x;
		oss << " ";
		oss << Enemies[7].position.z;
		oss << " ";
		oss << Enemies[7].bIsActive;

		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = (unsigned short)_strToSend.size();
	}

	void SerializeBullets(EMessageType _MessageType, char* _name, Bullet _bullet1, Bullet _bullet2, Bullet _bullet3, Bullet _bullet4, Bullet _bullet5, Bullet _bullet6, Bullet _bullet7, Bullet _bullet8, Bullet _bullet9, Bullet _bullet10) {

		MessageType = _MessageType;
		strcpy_s(Name, strlen(_name) + 1, _name);

		Bullets.clear();

		Bullets.push_back(_bullet1);
		Bullets.push_back(_bullet2);
		Bullets.push_back(_bullet3);
		Bullets.push_back(_bullet4);
		Bullets.push_back(_bullet5);
		Bullets.push_back(_bullet6);
		Bullets.push_back(_bullet7);
		Bullets.push_back(_bullet8);
		Bullets.push_back(_bullet9);
		Bullets.push_back(_bullet10);

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << Name;
		oss << " ";
		oss << Bullets[0].position.x;
		oss << " ";
		oss << Bullets[0].position.z;
		oss << " ";
		oss << Bullets[0].bIsActive;
		oss << " ";
		oss << Bullets[1].position.x;
		oss << " ";
		oss << Bullets[1].position.z;
		oss << " ";
		oss << Bullets[1].bIsActive;
		oss << " ";
		oss << Bullets[2].position.x;
		oss << " ";
		oss << Bullets[2].position.z;
		oss << " ";
		oss << Bullets[2].bIsActive;
		oss << " ";
		oss << Bullets[3].position.x;
		oss << " ";
		oss << Bullets[3].position.z;
		oss << " ";
		oss << Bullets[3].bIsActive;
		oss << " ";
		oss << Bullets[4].position.x;
		oss << " ";
		oss << Bullets[4].position.z;
		oss << " ";
		oss << Bullets[4].bIsActive;
		oss << " ";
		oss << Bullets[5].position.x;
		oss << " ";
		oss << Bullets[5].position.z;
		oss << " ";
		oss << Bullets[5].bIsActive;
		oss << " ";
		oss << Bullets[6].position.x;
		oss << " ";
		oss << Bullets[6].position.z;
		oss << " ";
		oss << Bullets[6].bIsActive;
		oss << " ";
		oss << Bullets[7].position.x;
		oss << " ";
		oss << Bullets[7].position.z;
		oss << " ";
		oss << Bullets[7].bIsActive;
		oss << " ";
		oss << Bullets[8].position.x;
		oss << " ";
		oss << Bullets[8].position.z;
		oss << " ";
		oss << Bullets[8].bIsActive;
		oss << " ";
		oss << Bullets[9].position.x;
		oss << " ";
		oss << Bullets[9].position.z;
		oss << " ";
		oss << Bullets[9].bIsActive;


		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = (unsigned short)_strToSend.size();
	}

	TPacket DeserializeEnemies(char* _PacketData) {

		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		Enemies.clear();

		Enemies.reserve(8);

		for (int i = 0; i < 8; i++) {
			Enemy temp;
			Enemies.push_back(temp);
		}

		iss >> this->MessageType;

		for (int i = 0; i < 8; i++) {

			iss >> this->Enemies[i].position.x;
			iss >> this->Enemies[i].position.z;
			iss >> this->bIsActive;

		}

		return *this;
	}

	TPacket DeserializeBullets(char* _PacketData) {

		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		Bullets.clear();

		Bullets.reserve(10);

		for (int i = 0; i < 10; i++) {
			Bullet temp;
			Bullets.push_back(temp);
		}

		iss >> this->MessageType;
		iss >> this->Name;

		for (int i = 0; i < 10; i++) {

			iss >> this->Bullets[i].position.x;
			iss >> this->Bullets[i].position.z;
			iss >> this->bIsActive;

		}

		return *this;
	}


	void SerializePowerUp(EMessageType _MessageType, PowerUp _Powerup) {

		MessageType = _MessageType;

		PowerUp1 = _Powerup;

		std::ostringstream oss;
		oss << MessageType;
		oss << " ";
		oss << PowerUp1.GetPosition().x;
		oss << " ";
		oss << PowerUp1.GetPosition().z;
		oss << " ";
		oss << PowerUp1.IsActive();
		oss << " ";

		std::string _strToSend = oss.str();
		const char* _pcToSend = _strToSend.c_str();

		strcpy_s(PacketData, strlen(_pcToSend) + 1, _pcToSend);

		PacketSize = (unsigned short)_strToSend.size();

	}

	TPacket DeserializePowerUp(char* _PacketData) {
		std::string _strTemp(_PacketData);
		std::istringstream iss(_strTemp);

		iss >> this->MessageType;

		iss >> this->PowerUp1.position.x;
		iss >> this->PowerUp1.position.z;
		iss >> this->PowerUp1.bIsActive;


		return *this;
	}

};

class INetworkEntity {
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