#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>

#define TIME_PORT	27015



bool isNumber(const string& str);
void closingConnection(SOCKET connSocket);
boolean sendQuestionToServer(int bytesSent, SOCKET connSocket, char sendBuff[255], sockaddr_in server);
boolean recvieAnswearFromServer(int bytesRecv, SOCKET connSocket, char recvBuff[255]);

void main()
{

	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a client to communicate on a network, it must connect to a server.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	// Send and receive data.

	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];

	// Asks the server what's the currnet time.
	// The send function sends data on a connected socket.
	// The buffer to be sent and its size are needed.
	// The fourth argument is an idicator specifying the way in which the call is made (0 for default).
	// The two last arguments hold the details of the server to communicate with. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	

	while (true)
	{
		cout << "Menu\n"
			<< "Please enter the number from the options:"
			<< endl << "1.  GetTime"
			<< endl << "2.  GetTimeWithoutDate"
			<< endl << "3.  GetTimeSinceEpoch"                        
			<< endl << "4.  GetClientToServerDelayEstimation"      
			<< endl << "5.  MeasureRTT"                            
			<< endl << "6.  GetTimeWithoutDateOrSeconds"
			<< endl << "7.  GetYear"
			<< endl << "8.  GetMonthAndDay"
			<< endl << "9.  GetSecondsSinceBeginingOfMonth"
			<< endl << "10. GetWeekOfYear"
			<< endl << "11.	GetDaylightSavings"                     
			<< endl << "12.	GetTimeWithoutDateInCity"
			<< endl << "13.	MeasureTimeLap"                        
			<< endl << "14. Exit"
			<< endl;

		string choice;
		int choiceInt;

		cin >> choice;

		if (isNumber(choice))
		{
			choiceInt = atoi(choice.c_str());

			if (choiceInt == 4)
			{
				strcpy_s(sendBuff, choice.data());
				DWORD sum = 0;

				for (int i = 0; i < 100; i++)
				{
					if (!sendQuestionToServer(bytesSent, connSocket, sendBuff, server))
					{
						closingConnection(connSocket);
						return;
					}
				}

				for (int i = 0; i < 100; i++)
				{
					bytesRecv = recv(connSocket, recvBuff, 255, 0);
					if (SOCKET_ERROR == bytesRecv)
					{
						cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
						closesocket(connSocket);
						WSACleanup();
						return;
					}

					recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
					sum += (DWORD)recvBuff;
				}

				sum /= 100;
				cout << "Time Client: Recieved: " /* << sum << " bytes of \""*/ << sum << "\" message.\n";
			}
			else if (choiceInt == 5)
			{
				strcpy_s(sendBuff, choice.data());
				float sum=0;

				long double temp1, temp2, res=0;

				for (int i = 0; i < 100; i++)
				{
					//clock_t start, end;
					//start = clock();
					temp1 = (double)GetTickCount64();
					if (!sendQuestionToServer(bytesSent, connSocket, sendBuff, server))
					{
						closingConnection(connSocket);
						return;
					}

					bytesRecv = recv(connSocket, recvBuff, 255, 0);
					if (SOCKET_ERROR == bytesRecv)
					{
						cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
						closesocket(connSocket);
						WSACleanup();
						return;
					}
					//end = clock();
					temp2 = (double)GetTickCount64();
					temp1 = (double)(temp2 - temp1);
					if (temp1 < 0) temp1 *= -1;
					res += temp1;
					//sum += ((float)end - start) / CLOCKS_PER_SEC;
				}

				res = (res / 100);
				res = res / 1000;
				//sum /= 100;
				cout << "Time Client: Recieved: " /* << bytesRecv << " bytes of \""*/ << res << "\" message.\n";
			}
			else if (choiceInt == 12)
			{
				cout << "Menu Time Zones\n"
					<< "please enter one of the folloing:"
					<< endl << "1. TOK"
					<< endl << "2. MEL"
					<< endl << "3. SAN"
					<< endl << "4. POR"
					<< endl << "5. The right universal time (UTC)"
					<< endl;

				string utc;
				cin >> utc;
				if (isNumber(utc))
				{
					int utcInt = atoi(utc.c_str());
					switch (utcInt)
					{
						case 1:
							strcpy_s(sendBuff, "12TOK");
							break;
						case 2:
							strcpy_s(sendBuff, "12MEL");
							break;
						case 3:
							strcpy_s(sendBuff, "12SAN");
							break;
						case 4:
							strcpy_s(sendBuff, "12POR");
							break;
						case 5:
							strcpy_s(sendBuff, "12UTC");
							break;
					}

					if (!sendQuestionToServer(bytesSent, connSocket, sendBuff, server))
					{
						closingConnection(connSocket);
						return;
					}
					if (!recvieAnswearFromServer(bytesRecv, connSocket, recvBuff))
					{
						closingConnection(connSocket);
						return;
					}
					cout << "\n\n";

				}
				else
				{
					cout << "Error with input";
				}
			}
			else if (choiceInt >= 1 && choiceInt <= 13)
			{
				strcpy_s(sendBuff, choice.data());
				if (!sendQuestionToServer(bytesSent, connSocket, sendBuff, server))
				{
					closingConnection(connSocket);
					return;
				}
				if (!recvieAnswearFromServer(bytesRecv, connSocket, recvBuff))
				{
					closingConnection(connSocket);
					return;
				}
				cout << "\n\n";
			}
			else if (choiceInt == 14)
			{
				closingConnection(connSocket);
				return;
			}
			else 
			{
				cout << "The number is out of range!";
			}	
		}
		else
		{
			cout << "You need to enter a number!";
		}
	}
}





bool isNumber(const string& str)
{
	for (char const& c : str) {
		if (std::isdigit(c) == 0) return false;
	}
	return true;
}

void closingConnection(SOCKET connSocket)
{
	// Closing connections and Winsock.
	cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);
	system("pause");
}

boolean sendQuestionToServer(int bytesSent, SOCKET connSocket, char sendBuff[255], sockaddr_in server)
{
	bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return false;
	}
	cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	return true;
}

boolean recvieAnswearFromServer(int bytesRecv, SOCKET connSocket, char recvBuff[255])
{
	// Gets the server's answer using simple recieve (no need to hold the server's address).
	bytesRecv = recv(connSocket, recvBuff, 255, 0);
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
		closesocket(connSocket);
		WSACleanup();
		return false;
	}

	recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
	cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
	return true;
}
