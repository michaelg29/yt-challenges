#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <process.h>
#include <stdatomic.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFLEN 512
#define PORT 27015
#define ADDRESS "127.0.0.1" // localhost

// global running variable
_Atomic char running = 0; // default false

DWORD WINAPI sendThreadFunc(LPVOID lpParam);

int main()
{
    printf("Hello, world!\n");

    int res;

    // INITIALIZATION ===========================
    WSADATA wsaData;
    res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res)
    {
        printf("Startup failed: %d\n", res);
        return 1;
    }
    // ==========================================

    // SETUP CLIENT SOCKET ======================

    // construct socket
    SOCKET client;
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client == INVALID_SOCKET)
    {
        printf("Error with construction: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // connect to address
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ADDRESS);
    address.sin_port = htons(PORT);
    res = connect(client, (struct sockaddr *)&address, sizeof(address));
    if (res == SOCKET_ERROR)
    {
        printf("Connect failed: %d\n", WSAGetLastError());
        closesocket(client);
        WSACleanup();
        return 1;
    }
    else if (client == INVALID_SOCKET)
    {
        printf("Connect failed: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // set as running
    printf("Successfully connected to %s:%d\n", ADDRESS, PORT);
    running = !0; // true

    // ==========================================

    // MAIN LOOP ================================

    // start send thread
    DWORD thrdId;
    HANDLE sendThread = CreateThread(
        NULL,
        0,
        sendThreadFunc,
        &client,
        0,
        &thrdId);

    if (sendThread)
    {
        printf("Send thread started with thread ID: %d\n", thrdId);
    }
    else
    {
        printf("Send thread failed: %d\n", GetLastError());
    }

    // receive loop
    char recvbuf[BUFLEN];
    do
    {
        res = recv(client, recvbuf, BUFLEN, 0);
        recvbuf[res] = '\0';
        if (res > 0)
        {
            printf("Received (%d): %s\n", res, recvbuf);
        }
        else if (!res)
        {
            printf("Connection closed.\n");
            running = 0;
        }
        else
        {
            printf("Receive failed: %d\n", WSAGetLastError());
            running = 0;
        }
    } while (running && res > 0);
    running = 0;

    // connection finished, terminate send thread
    if (CloseHandle(sendThread))
    {
        printf("Send thread closed successfully.\n");
    }

    // ==========================================

    // CLEANUP ==================================

    res = shutdown(client, SD_BOTH);
    if (res == SOCKET_ERROR)
    {
        printf("Shutdown failed: %d\n", WSAGetLastError());
        closesocket(client);
        WSACleanup();
        return 1;
    }

    closesocket(client);
    WSACleanup();

    // ==========================================

    return 0;
}

DWORD WINAPI sendThreadFunc(LPVOID lpParam)
{
    SOCKET client = *(SOCKET *)lpParam;

    char sendbuf[BUFLEN];
    int sendbuflen, res;

    while (running)
    {
        scanf("%s", sendbuf);

        if (!running)
        {
            break;
        }

        sendbuflen = strlen(sendbuf);
        res = send(client, sendbuf, sendbuflen, 0);

        if (res != sendbuflen)
        {
            printf("Send failed.");
            break;
        }
        else if (!memcmp(sendbuf, "/leave", 6))
        {
            running = 0;
            break;
        }
    }

    return 0;
}