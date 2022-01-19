#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include <sstream>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include<windows.h>
#include <cstdlib>

#define TIME_PORT	27015

//Time Zones Constants
#define TOK (+9)
#define MEL (+11)
#define SAN (-8)
#define POR (0)
#define UTC (0)


bool isNumber(const string& str);


void main()
{



	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	// Server side:
	// Create and bind a socket to an internet address.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a server to communicate on a network, it must first bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Waits for incoming requests from clients.

	// Send and receive data.
	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];

	clock_t start = clock(), end = clock();//case13
	int countCase13 = 0;
	
	int currentResponeCase4 = 0;

	// Get client's requests and answer them.
	// The recvfrom function receives a datagram and stores the source address.
	// The buffer for data to be received and its available size are 
	// returned by recvfrom. The fourth argument is an idicator 
	// specifying the way in which the call is made (0 for default).
	// The two last arguments are optional and will hold the details of the client for further communication. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		// Answer client's request by the current time.


		time_t timer;
		time(&timer);
		struct tm* tm = localtime(&timer);
		struct tm* globalTime = gmtime(&timer);

		int choice;

		if (isNumber((string)recvBuff))
		{
			choice = atoi(((string)recvBuff).c_str());
			switch (choice)
			{
			case 1://GetTime
			{

				strcpy(sendBuff, ctime(&timer));

				//time_t timer;
				//time(&timer);
				//// Parse the current time to printable string.
				//strcpy(sendBuff, ctime(&timer));
				//sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string
			}
			break;

			case 2://GetTimeWithoutDate
			{

				strftime(sendBuff, 255, "%X", tm);


				//time_t currentTime;
				//struct tm* localTime;
				//time(&currentTime);                   // Get the current time
				//localTime = localtime(&currentTime);  // Convert the current time to the local time
				//int Hour = localTime->tm_hour;
				//int Min = localTime->tm_min;
				//int Sec = localTime->tm_sec;
				//string tt = to_string(Hour) + ":" + to_string(Min) + ":" + to_string(Sec);
				//strcpy(sendBuff, tt.data());	
			}
			break;

			case 3://GetTimeSinceEpoch 
			{
				time_t seconds_past_epoch = time(0);
				ostringstream oss;
				oss << seconds_past_epoch;
				strcpy(sendBuff, oss.str().data());
			}
			break;

			case 4://GetClientToServerDelayEstimation
			{
				currentResponeCase4++;

				if (currentResponeCase4 == 100)
				{
					for (int i = 0; i < 100; i++)
					{
						DWORD tick = GetTickCount64();
						ostringstream oss;
						oss << tick;
						strcpy(sendBuff, oss.str().data());


						// Sends the answer to the client, using the client address gathered
						// by recvfrom. 
						bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
						if (SOCKET_ERROR == bytesSent)
						{
							cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
							closesocket(m_socket);
							WSACleanup();
							return;
						}

						cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";

					}
				}
			}
			break;

			case 5://MeasureRTT
				strcpy(sendBuff, "test for 5");
				break;

			case 6://GetTimeWithoutDateOrSeconds
				strftime(sendBuff, 255, "%H:%M", tm);
				break;

			case 7://GetYear
				strftime(sendBuff, 255, "%Y", tm);
				break;


			case 8://GetMonthAndDay 
				strftime(sendBuff, 255, "%B, %A", tm);
				break;

			case 9://GetSecondsSinceBeginingOfMonth  
			{
				char secondsArray[2];
				strftime(secondsArray, 2, "%e", tm);
				int seconds = atoi(secondsArray);
				seconds *= 24 * 60 * 60;

				time_t currentTime;
				struct tm* localTime;
				time(&currentTime);                   // Get the current time
				localTime = localtime(&currentTime);  // Convert the current time to the local time
				int Hour = localTime->tm_hour;
				int Min = localTime->tm_min;
				int Sec = localTime->tm_sec;

				char num_char[10];
				seconds += Sec + Hour * 60 * 60 + Min * 60;
				sprintf(num_char, "%d", seconds);
				strcpy(sendBuff, num_char);
			}
				break;

			case 10://GetWeekOfYear 
				strftime(sendBuff, 255, "%W", tm);
				break;

			case 11:
			{
				struct tm day_light;
				localtime_s(&day_light, &timer);
				int isDayLightSaving = day_light.tm_isdst;
				if (isDayLightSaving > 0)
					strcpy(sendBuff, "1");
				else
					strcpy(sendBuff, "0"); 
			}
				break;

			case 13:
			{
				countCase13++;

				if (countCase13 % 2 == 0)
				{
					end = clock();
					double d = ((double)start - end) / CLOCKS_PER_SEC;
					if (d < 0) d *= -1;

					if (d > 180)
					{
						countCase13 = 1;
						start = clock();
						strcpy(sendBuff, "Measurement turned on");
					}
					else
					{
						strcpy(sendBuff, (to_string(d).c_str()));
					}
				}
				else
				{
					start = clock();
					strcpy(sendBuff, "Measurement turned on");
				}
			}
				break;
			}
		}
		else
		{
			if (recvBuff[0] == '1' && recvBuff[1] == '2')//case12
			{
				int num1;
				int num2 = globalTime->tm_min;
				int num3 = globalTime->tm_sec;

				string h;
				string m;
				string s;

				stringstream hSs1;
				stringstream hSs2;
				stringstream hSs3;

				hSs2 << num2;
				hSs2 >> m;

				hSs3 << num3;
				hSs3 >> s;

				if(strcmp(recvBuff, "12TOK") == 0)
				{

					num1 = (globalTime->tm_hour + TOK) % 24;			
				}
				else if (strcmp(recvBuff, "12MEL") == 0)
				{
					num1 = (globalTime->tm_hour + MEL) % 24;
				}
				else if (strcmp(recvBuff, "12SAN") == 0)
				{
					num1 = (globalTime->tm_hour + SAN) % 24;
				}
				else if (strcmp(recvBuff, "12POR") == 0)
				{
					num1 = (globalTime->tm_hour + POR) % 24;
				}
				else if (strcmp(recvBuff, "12UTC") == 0)
				{
					num1 = (globalTime->tm_hour + UTC) % 24;										
				}

				hSs1 << num1;
				hSs1 >> h;
				strcpy(sendBuff, (h + ":" + m + ":" + s).c_str());
			}
		}

		
		if (choice != 4)
		{

			// Sends the answer to the client, using the client address gathered
			// by recvfrom. 
			bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
			if (SOCKET_ERROR == bytesSent)
			{
				cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
				closesocket(m_socket);
				WSACleanup();
				return;
			}

			cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
		}
		}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}




bool isNumber(const string& str)
{
	for (char const& c : str) {
		if (std::isdigit(c) == 0) return false;
	}
	return true;
}