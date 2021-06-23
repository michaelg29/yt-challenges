#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFLEN 512
#define PORT 27015
#define ADDRESS "127.0.0.1" // aka "localhost"

int main()
{
    printf("Hello, world!\n");

    int res, sendRes;

    // INITIALIZATION ===========================
    WSADATA wsaData; // configuration data
    res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res)
    {
        printf("Startup failed: %d\n", res);
        return 1;
    }
    // ==========================================

    // SETUP SERVER =============================

    // construct socket
    SOCKET listener;
    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET)
    {
        printf("Error with construction: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // bind to address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ADDRESS);
    address.sin_port = htons(PORT);
    res = bind(listener, (struct sockaddr *)&address, sizeof(address));
    if (res == SOCKET_ERROR)
    {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    // set as a listener
    res = listen(listener, SOMAXCONN);
    if (res == SOCKET_ERROR)
    {
        printf("Listen failed: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }
    // ==========================================

    printf("Accepting on %s:%d\n", ADDRESS, PORT);

    // HANDLE A CLIENT ==========================

    // accept client socket
    SOCKET client;
    struct sockaddr_in clientAddr;
    int clientAddrlen;
    client = accept(listener, NULL, NULL);
    if (client == INVALID_SOCKET)
    {
        printf("Could not accept: %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    // get client information
    getpeername(client, (struct sockaddr *)&clientAddr, &clientAddrlen);
    printf("Client connected at %s:%d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    // send welcome message
    char *welcome = "Welcome to the server :)";
    sendRes = send(client, welcome, strlen(welcome), 0);
    if (sendRes != strlen(welcome))
    {
        printf("Error sending: %d\n", WSAGetLastError());
        shutdown(client, SD_BOTH);
        closesocket(client);
    }

    // receive messages
    char recvbuf[BUFLEN];
    do
    {
        res = recv(client, recvbuf, BUFLEN, 0);
        if (res > 0)
        {
            recvbuf[res] = '\0';
            printf("Message received (%d): %s\n", res, recvbuf);

            if (!memcmp(recvbuf, "/quit", 5 * sizeof(char)))
            {
                // received quit command
                printf("Closing connection.\n");
                break;
            }

            // echo message back
            sendRes = send(client, recvbuf, res, 0);
            if (sendRes != res)
            {
                printf("Error sending: %d\n", WSAGetLastError());
                shutdown(client, SD_BOTH);
                closesocket(client);
                break;
            }
        }
        else if (!res)
        {
            // client disconnected
            printf("Closing connection.\n");
            break;
        }
        else
        {
            printf("Receive failed: %d\n", WSAGetLastError());
            break;
        }
    } while (res > 0);

    // shutdown client
    res = shutdown(client, SD_BOTH);
    if (res == SOCKET_ERROR)
    {
        printf("Client shutdown failed: %d\n", WSAGetLastError());
    }
    closesocket(client);

    // ==========================================

    // CLEANUP ==================================
    // shut down server socket
    closesocket(listener);

    // cleanup WSA
    res = WSACleanup();
    if (res)
    {
        printf("Cleanup failed: %d\n", res);
        return 1;
    }
    // ==========================================

    printf("Shutting down.\nGood night.\n");

    return 0;
}