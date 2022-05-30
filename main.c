#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#ifdef _WIN32
#include <ws2tcpip.h>
#include <Winsock2.h>
#include <ws2def.h>
#include <synchapi.h>
#else //_WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#define SOCKET_ERROR SO_ERROR
#define INVALID_SOCKET (-1)
#endif //_WIN32

#define xstr(s) str(s)
#define str(s) #s

#ifndef SERVER_IP
#define SERVER_IP ("127.0.0.1")
#endif //SERVER_IP

#ifndef DEST_IP
#define DEST_IP ("127.0.0.1")
#endif //DEST_IP

#ifndef DEST_PORT
#define DEST_PORT 8080
#endif //DEST_PORT

#ifndef SERVER_PORT
#define SERVER_PORT 1234
#endif //SERVER_PORT

//Disables two warnings for this code, makes compilation look cleaner
#pragma warning(disable : 6308 4133)

typedef int BOOL;
#define FALSE 0
#define TRUE 1

int main() {
	//declaring/defining all local variables
	int err = 0, numbytes = 0;
#ifdef _WIN32
	SOCKET listen_sockfd, new_listen_sockfd, send_sockfd;
	SOCKADDR_IN listen_address, dest_address;
	WSADATA wsaData;
#else //_WIN32
	int listen_sockfd, new_listen_sockfd, send_sockfd;
	struct sockaddr_in listen_address, dest_address;
#endif //_WIN32
	unsigned int recvPeekBuf = 0, recvPeekBufValue = 0;
	const char yes = 1;
	struct in_addr listen_inaddr, send_inaddr;
	BOOL bIsDoneRecv = FALSE, bIsDoneSend = FALSE, bServerConnectionClosed = FALSE, bClientConnectionClosed = FALSE;
	size_t memorySize = 4096, totalNumBytes = 0, sin_size = 0;
	
	//Dynamic memory allocation
	char* data = (char*)malloc(memorySize);
	if (data == NULL)
	{
#ifdef _DEBUG
		printf("[!] Unable to allocate memory for data buffer.\nQUIT\n");
#endif //_DEBUG
		exit(1);
	}

	(void)inet_pton(AF_INET, xstr(SERVER_IP), &listen_inaddr);
	(void)inet_pton(AF_INET, xstr(DEST_IP), &send_inaddr);

	listen_address.sin_family = AF_INET;
	listen_address.sin_addr = listen_inaddr;
	listen_address.sin_port = htons(SERVER_PORT);

	dest_address.sin_family = AF_INET;
	dest_address.sin_addr = send_inaddr;
	dest_address.sin_port = htons(DEST_PORT);

#ifdef _WIN32
	(void)WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif //_WIN32

	listen_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	send_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef _DEBUG
	printf("listen_sockfd: %d\n", listen_sockfd);
	printf("send_sockfd: %d\n", send_sockfd);
#endif //_DEBUG

	//This will cause incompatible pointer type warnings because
	//listen_address is not const.
	err = bind(listen_sockfd, &listen_address, sizeof(listen_address));
#ifdef _DEBUG
	printf("bind() err: %d\n", err);
	if (err == SOCKET_ERROR)
	{
#ifdef _WIN32
		printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
		printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
	}
#endif //_DEBUG

	
	err = listen(listen_sockfd, SOMAXCONN);
#ifdef _DEBUG
	printf("listen() err: %d\n", err);
	if (err == SOCKET_ERROR)
	{
#ifdef _WIN32
		printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
		printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
	}
#endif //_DEBUG
	new_listen_sockfd = accept(listen_sockfd, NULL, NULL);
#ifdef _DEBUG
	printf("new_listen_fd: %d\n", new_listen_sockfd);
	if (err == SOCKET_ERROR)
	{
#ifdef _WIN32
		printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
		printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
	}
#endif //_DEBUG

	//This will cause incompatible pointer type warnings because
	// dest_address is not const.
	err = connect(send_sockfd, &dest_address, sizeof(dest_address));
#ifdef _DEBUG
	printf("connect() err: %d\n", err);
	if (err == SOCKET_ERROR)
	{
#ifdef _WIN32
		printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
		printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
	}
#endif
	
	while (TRUE)
	{
		if (bServerConnectionClosed)
		{
			send_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			err = connect(send_sockfd, &dest_address, sizeof(dest_address));
#ifdef _DEBUG
			printf("connect() err: %d\n", err);
#endif //_DEBUG, can't wrap whole comparison becuase of bServerConnectionClosed assignment below.
			if (err == SOCKET_ERROR)
			{
#ifdef _WIN32
				printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
				printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
			}
			else
			{
				bServerConnectionClosed = FALSE;
			}
			

		}

		if (bClientConnectionClosed)
		{
			err = listen(listen_sockfd, SOMAXCONN);
#ifdef _DEBUG
			printf("listen() err: %d\n", err);
			if (err == SOCKET_ERROR)
			{
#ifdef _WIN32
				printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
				printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
			}
#endif //_DEBUG

			new_listen_sockfd = accept(listen_sockfd, NULL, NULL);
#ifdef _DEBUG
			printf("new_listen_fd: %d\n", new_listen_sockfd);
			if (err == SOCKET_ERROR)
			{
#ifdef _WIN32
				printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
				printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
			}
#endif //_DEBUG

			if (new_listen_sockfd != INVALID_SOCKET)
			{
				bClientConnectionClosed = FALSE;
			}
		}

		//Start loop to receive all data from client
		while (TRUE)
		{
			if (data != NULL)
			{
				numbytes = recv(new_listen_sockfd,
					//If the memory size is still 4096, we dont need to adjust
					(totalNumBytes == 0 ? data : data + (memorySize / 2)),
					//Start at size 4096, but if it's been doubled, we need to append
					// data to the data array and only use memorySize/2
					(memorySize == 4096 ? memorySize : memorySize / 2),
					0);
			}
			else 
			{
#ifdef _DEBUG
				printf("[!] Unable to allocate memory for data buffer.\nQUIT\n");
#endif
				exit(1);
			}
			
#ifdef _DEBUG
			printf("bytes recv() from client: %d\n", numbytes);
#endif
			if (numbytes == SOCKET_ERROR)
			{
#ifdef _DEBUG
#ifdef _WIN32
				printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
				printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
#endif //_DEBUG
			}
			else if (numbytes == 0)
			{
#ifdef _WIN32
				closesocket(new_listen_sockfd);
#else //_WIN32
				close(new_listen_sockfd);
#endif //_WIN32
				bClientConnectionClosed = TRUE;
#ifdef _DEBUG
				printf("[!] Client closed connection.\n");
#endif //_DEBUG
				break;
			}
			else {

				if (numbytes < memorySize)
				{
					bIsDoneRecv = TRUE;
					totalNumBytes = numbytes;
				}
				else if (numbytes == memorySize)
				{
					//Too much data, double size of data array
					memorySize *= 2;
					if (data != NULL)
					{
						data = realloc(data, memorySize);
					}
					if (data == NULL)
					{
#ifdef _DEBUG
						printf("[!] Unable to allocate memory for data buffer.\nQUIT\n");
#endif //_DEBUG						
						exit(1);
					}
					totalNumBytes += numbytes;
				}
				else
				{
#ifdef _DEBUG
					printf("[!] Somehow received more data than possible...\nQUIT.\n");
#endif //_DEBUG
					exit(1);
				}
			}

			if (bIsDoneRecv) break;
		}

		if (!bClientConnectionClosed)
		{
			//Send all data to destination at once
			numbytes = send(send_sockfd, data, totalNumBytes, 0);
#ifdef _DEBUG
			printf("bytes send() to dest: %d\n", numbytes);
#endif //_DEBUG
			if (numbytes == SOCKET_ERROR)
			{
#ifdef _WIN32
				printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
				printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
			}
		}
		memset(data, 0, memorySize);
		totalNumBytes = 0;

		//start receiving all data from destination
		while (TRUE)
		{
			if (data != NULL)
			{
#ifdef _DEBUG
				printf("recv(): send_sockfd=%d, data=%p, memorySize=%ld\n", send_sockfd, data, memorySize);
				printf("data: 0x%x 0x%x 0x%x 0x%x 0x%x\n", data[0], data[1], data[2], data[3], data[4]);
#endif //_DEBUG				
				numbytes = recv(send_sockfd, data, memorySize, 0);
			}
			else
			{
#ifdef _DEBUG
				printf("[!] Unable to allocate data storage...\nQUIT.\n");
#endif //_DEBUG
				exit(1);
			}
			
#ifdef _DEBUG
			printf("bytes recv() from dest: %d\n", numbytes);
#endif //_DEBUG
			if (numbytes == SOCKET_ERROR)
			{
#ifdef _DEBUG
#ifdef _WIN32
				printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
				printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
#endif //_DEBUG
			}
			else if (numbytes == 0) //connection closed from the server
			{
#ifdef _WIN32
				closesocket(send_sockfd);
#else //_WIN32
				close(send_sockfd);
#endif //_WIN32
				bServerConnectionClosed = TRUE;
#ifdef _DEBUG
				printf("[!] Server closed connection.\n");
#endif //_DEBUG
				break;
			}
			else
			{

				if (numbytes < memorySize)
				{
					totalNumBytes = numbytes;
					recvPeekBufValue = recv(send_sockfd, &recvPeekBuf, sizeof(recvPeekBuf), MSG_PEEK);
					if (recvPeekBufValue <= 0)
					{
						break;
					}
#ifdef _DEBUG
					printf("multiple, small packets, sending...\n");
#endif //_DEBUG
					numbytes = send(new_listen_sockfd, data, totalNumBytes, 0);
					memset(data, 0, memorySize); //reset for next small packet
				}
				else if (numbytes == memorySize)
				{
					//Too much data, double size of data array
					memorySize *= 2;
					if (data != NULL)
					{
						data = realloc(data, memorySize);
					}
					if (data == NULL)
					{
#ifdef _DEBUG
						printf("[!] Unable to allocate memory for data buffer.\nQUIT\n");
#endif //_DEBUG						
						exit(1);
					}
					totalNumBytes += numbytes;
				}
				else
				{
#ifdef _DEBUG
					printf("[!] Somehow received more data than possible...\nQUIT.\n");
#endif //_DEBUG					
					exit(1);
				}
			}
		}

		if (!bServerConnectionClosed)
		{
			//send all data to client
#ifdef _DEBUG
			printf("send(): new_listen_sockfd=%d, data=%p, totalNumBytes=%ld\n", new_listen_sockfd, data, totalNumBytes);
			printf("data: 0x%x 0x%x 0x%x 0x%x 0x%x\n", data[0], data[1], data[2], data[3], data[4]);
#endif //_DEBUG			
			numbytes = send(new_listen_sockfd, data, totalNumBytes, 0);
#ifdef _DEBUG
			printf("bytes send() to client: %d\n", numbytes);
			if (numbytes == SOCKET_ERROR)
			{
#ifdef _WIN32
				printf("WSAGetLastError(): %d\n", WSAGetLastError());
#else //_WIN32
				printf("errno: %d, %s\n", errno, strerror(errno));
#endif //_WIN32
			}
#endif //_DEBUG
		}
		
		//Reset all variables
		totalNumBytes = 0;
		bIsDoneRecv = FALSE;
		bIsDoneSend = FALSE;
		memset(data, 0, memorySize);
	}

	free(data);

#ifdef _WIN32
	(void)WSACleanup();
#endif //_WIN32
	return 0;
}