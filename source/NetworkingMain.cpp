////
//// Bachelor of Software Engineering
//// Media Design School
//// Auckland
//// New Zealand
////
//// (c) 2015 Media Design School
////
//// File Name	: main.cpp
//// Description	: main implementation file
//// Author		: Mitchell Currie
//// Mail			: Mitchell.Currie@mediadesign.school.nz
////
//
////Library Includes
//#include <Windows.h>
//#include <cassert>
////#include <vld.h>
//#include <thread>
//#include <ctime>
//
////Local Includes
//#include "consoletools.h"
//#include "network.h"
//#include "client.h"
//#include "server.h"
//#include "InputLineBuffer.h"
//#include <functional>
//
//// make sure the winsock lib is included...
//#pragma comment(lib,"ws2_32.lib")
//
//void stopCursor();
//bool Quit(char _packetData[60]);
//
//int main()
//{
//	char* _pcPacketData = 0; //A local buffer to receive packet data into
//	_pcPacketData = new char[MAX_MESSAGE_LENGTH];
//	//ZeroMemory(&_pcPacketData, strlen(_pcPacketData));
//	strcpy_s(_pcPacketData, strlen("") + 1, "");
//
//	char _cIPAddress[MAX_ADDRESS_LENGTH]; // An array to hold the IP Address as a string
//	//ZeroMemory(&_cIPAddress, strlen(_cIPAddress));
//
//	unsigned char _ucChoice;
//	EEntityType _eNetworkEntityType;
//	CInputLineBuffer _InputBuffer(MAX_MESSAGE_LENGTH);
//	std::thread _ReceiveClientThread;
//	std::thread _ReceiveServerThread;
//
//	//Get the instance of the network
//	CNetwork& _rNetwork = CNetwork::GetInstance();
//	_rNetwork.StartUp();
//
//	//A pointer to hold a client instance and server instance
//	CClient* _pClient;
//	CServer* _pServer;
//
//	// query, is this to be a client or a server?
//	_ucChoice = QueryOption("Do you want to run a client or server (C/S)?", "CS");
//	switch (_ucChoice)
//	{
//	case 'C':
//	{
//		_eNetworkEntityType = CLIENT;
//		break;
//	}
//	case 'S':
//	{
//		_eNetworkEntityType = SERVER;
//		break;
//	}
//	default:
//	{
//		std::cout << "This is not a valid option" << std::endl;
//		return 0;
//		break;
//	}
//	}
//
//	if (!_rNetwork.GetInstance().Initialise(_eNetworkEntityType))
//	{
//		std::cout << "Unable to initialise the Network........Press any key to continue......";
//		_getch();
//		return 0;
//	}
//
//	//Run receive on a separate thread so that it does not block the main client thread.
//	if (_eNetworkEntityType == CLIENT) //if network entity is a client
//	{
//
//		_pClient = static_cast<CClient*>(_rNetwork.GetInstance().GetNetworkEntity());
//		_ReceiveClientThread = std::thread(&CClient::ReceiveData, _pClient, std::ref(_pcPacketData));
//
//	}
//
//	if (_eNetworkEntityType == SERVER) //if network entity is a server
//	{
//
//		_pServer = static_cast<CServer*>(_rNetwork.GetInstance().GetNetworkEntity());
//		_ReceiveServerThread = std::thread(&CServer::ReceiveData, _pServer, std::ref(_pcPacketData));
//
//	}
//
//	// replicate for server with diff receive thread name
//	// Change the server receive function to have a while loop, pushing items to the server work queue
//
//	stopCursor();
//	bool bClientQuit = false;
//
//	// Timer for keep alive messages
//
//	/////////////////////////////  Timer  //////////////////////////////////////////
//	//	static std::clock_t start;
//	//	static double duration;
//
//
//
//	/////////////////////////////////////////////////////////////////////
//
//	while ((_rNetwork.IsOnline()) && !bClientQuit)
//	{
//		if (_eNetworkEntityType == CLIENT) //if network entity is a client
//		{
//			_pClient = static_cast<CClient*>(_rNetwork.GetInstance().GetNetworkEntity());
//
//			//Prepare for reading input from the user
//			_InputBuffer.PrintToScreenTop();
//
//			//Get input from the user
//			if (_InputBuffer.Update())
//			{
//				// we completed a message, lets send it:
//				int _iMessageSize = strlen(_InputBuffer.GetString());
//
//				//Put the message into a packet structure
//				TPacket _packet;
//				//_packet.Serialize(DATA, _pClient->GetName(), const_cast<char*>(_InputBuffer.GetString())); //Hardcoded username; change to name as taken in via user input.
//				char* Message = const_cast<char*>(_InputBuffer.GetString());
//
//				if (Quit(Message))
//				{
//					_packet.Serialize(QUIT, _pClient->GetName(), const_cast<char*>(_InputBuffer.GetString())); // Send quit message to server
//					std::cout << "You have left the chat. Please close the window." << std::endl;
//					// return(0);
//					bClientQuit = true;
//				}
//
//				else
//				{
//					// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
//
//					_packet.Serialize(DATA, _pClient->GetName(), const_cast<char*>(_InputBuffer.GetString()));
//				}
//
//				_rNetwork.GetInstance().GetNetworkEntity()->SendData(_packet.PacketData);
//
//				//Clear the Input Buffer
//				_InputBuffer.ClearString();
//				//Print To Screen Top
//				//	_InputBuffer.PrintToScreenTop();
//
//				// std::cout << "Completed a message" << std::endl;										
//			}
//
//			if (_pClient->HasGameStarted())
//			{
//				// Every loop for the client once the game has started, the message below is sent to the client with command "P"
//				// When it gets that command, it sends its position to the server
//				// Then when the server receives that position in server process case "P", it will update the players position in the vector of players
//
//				TPacket _packet;
//				_packet.Serialize(COMMAND, _pClient->GetName(), "P");
//				//	_pServer = static_cast<CServer*> (_rNetwork.GetInstance().GetNetworkEntity());
//				//	_pServer->ProcessData(_packet.PacketData); 
//
//				_pClient->GetWorkQueue()->push(_packet.PacketData);// Send start game message to server
//			}
//
//			//If the message queue is not empty 
//			if (!_pClient->GetWorkQueue()->empty())
//				//{
//				//	std::cout << "No message received from the server" << std::endl;
//				//}
//				//else
//			{
//
//				// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 9);
//
//				//Retrieve off a message from the queue
//				_pClient->GetWorkQueue()->pop(_pcPacketData);
//
//				std::string _strTemp(_pcPacketData);
//
//				_pClient->Process(_pcPacketData);
//
//			}
//
//		}
//		else //  (_eNetworkEntityType == SERVER)
//		{
//			//The server blocks if no data arrives from a client
//
//			_pServer = static_cast<CServer*> (_rNetwork.GetInstance().GetNetworkEntity());
//
//			//		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
//
//			///// KEEP ALIVE //////////////////////////////////
//
//			//if (duration > 0.5f)   // Keep alive message send once every second to server
//			//{
//			//	TPacket _packet;
//
//			//	_packet.Serialize(KEEPALIVE, "Server", " ");
//			////	_pServer = static_cast<CServer*> (_rNetwork.GetInstance().GetNetworkEntity());
//			//	//	_pServer->ProcessData(_packet.PacketData); 
//
//			//	_pServer->GetWorkQueue()->push(_packet.PacketData);// Send keep alive message to server
//
//			//	//	_pServer->ProcessData("2 Server No Message");
//			//	// std::cout << "Keep Alive" << std::endl;
//
//			//	start = std::clock(); // Reset clock start time
//			//}
//
//			///////////////  GAME START   ///////////////////////////////
//
//			if ((_pServer->HasGameStarted()) && (!(_pServer->HasNotifiedPlayers())) && (_pServer->GetNumberOfPlayers() > 1))
//			{
//				TPacket _packet;
//
//				_packet.Serialize(COMMAND, "Server", "G");
//				//	_pServer = static_cast<CServer*> (_rNetwork.GetInstance().GetNetworkEntity());
//				//	_pServer->ProcessData(_packet.PacketData); 
//
//				_pServer->GetWorkQueue()->push(_packet.PacketData);// Send start game message to server
//
//				_pServer->SetNotifiedPlayers(true);
//			}
//			//////////////////////////////////////////////////
//
//
//			if (!_pServer->GetWorkQueue()->empty())
//			{
//				// pop off server work queue
//				_pServer->GetWorkQueue()->pop(_pcPacketData);
//
//				//	_pServer->ReceiveData(_pcPacketData); // remove this
//				//	_pServer->GetRemoteIPAddress(_cIPAddress); // Needed?
//
//				//	std::string _strTemp(_pcPacketData);
//				_pServer->ProcessData(_pcPacketData);
//			}
//
//		}
//
//	} //End of while network is Online
//
//	_ReceiveClientThread.join();
//	_ReceiveServerThread.join();
//
//	//Shut Down the Network
//	_rNetwork.ShutDown();
//	_rNetwork.DestroyInstance();
//
//	delete[] _pcPacketData;
//	return 0;
//}
//
//void stopCursor()
//{
//	// Stops the cursor flashing - for the flashing light loop and lever movements.
//	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//	CONSOLE_CURSOR_INFO CursoInfo;
//	CursoInfo.dwSize = 1;
//	CursoInfo.bVisible = false;
//	SetConsoleCursorInfo(hConsole, &CursoInfo);
//}
//
//bool Quit(char _packetData[60])
//{
//	if ((_packetData[0] == '!') && ((_packetData[1] == 'q') || (_packetData[1] == 'Q')))
//		return true;
//
//	else
//		return false;
//}