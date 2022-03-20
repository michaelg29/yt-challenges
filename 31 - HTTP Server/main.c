#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFLEN 1024
#define ADDRESS "127.0.0.1"
#define PORT 5500

void cleanup(SOCKET listener);
int readFile(const char *filename, char **output);

int main()
{
    printf("Hello, world!\n");

    int res, sendRes;
    int running;
    WSADATA wsaData;
    SOCKET listener, client;
    struct sockaddr_in address, clientAddr;
    char recvbuf[BUFLEN];
    char *inputFileContents;
    int inputFileLength;

    // initialization
    res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res)
    {
        printf("Startup failed\n");
        return 1;
    }

    // setup server
    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET)
    {
        printf("Error with construction\n");
        cleanup(0);
        return 1;
    }

    // bind server
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ADDRESS);
    address.sin_port = htons(PORT);
    res = bind(listener, (struct sockaddr *)&address, sizeof(address));
    if (res == SOCKET_ERROR)
    {
        printf("Bind failed\n");
        cleanup(listener);
        return 1;
    }

    // set as listener
    res = listen(listener, SOMAXCONN);
    if (res == SOCKET_ERROR)
    {
        printf("Listen failed\n");
        cleanup(listener);
        return 1;
    }

    // load file
    inputFileLength = readFile("input.html", &inputFileContents);
    if (!inputFileLength || !inputFileContents)
    {
        printf("Could not read input form\n");
        cleanup(listener);
        return 1;
    }
    printf("File length: %d\n", inputFileLength);

    // done setting up
    printf("Accepting on %s:%d\n", ADDRESS, PORT);
    running = 1;

    while (running)
    {
        // accept client
        int clientAddrLen;
        client = accept(listener, NULL, NULL);
        if (client == INVALID_SOCKET)
        {
            printf("Could not accept\n");
            cleanup(listener);
            return 1;
        }

        // get client info
        getpeername(client, (struct sockaddr *)&clientAddr, &clientAddrLen);
        printf("Client connected at: %s:%d\n", inet_ntoa(address.sin_addr));

        // receive
        res = recv(client, recvbuf, BUFLEN, 0);
        if (res > 0)
        {
            // print message
            recvbuf[res] = 0;
            // printf("%s\n", recvbuf);

            // test if GET command
            if (!memcmp(recvbuf, "GET", 3))
            {
                printf("GET\n");
                sendRes = send(client, inputFileContents, inputFileLength, 0);
                if (sendRes == SOCKET_ERROR)
                {
                    printf("Send failed\n");
                }
            }
            // test if POST command
            else if (!memcmp(recvbuf, "POST", 4))
            {
                printf("POST\n");
                sendRes = send(client, inputFileContents, inputFileLength, 0);
                if (sendRes == SOCKET_ERROR)
                {
                    printf("Send failed\n");
                }

                // parse message
                // find equals sign
                int i = res - 1;
                for (; i >= 0; i--)
                {
                    if (recvbuf[i] == '=')
                    {
                        i++;
                        break;
                    }
                }
                // content from cursor onwards contains data
                printf("Received: %s\n", recvbuf + i);

                // get length
                int len = 0;
                for (int j = i; j < res; j++)
                {
                    len++;
                    if (recvbuf[j] == '%')
                    {
                        j += 2;
                    }
                }

                // read characters
                char *msg = malloc(len + 1);
                for (int cursor = 0, j = i; cursor < len; cursor++, j++)
                {
                    char c = recvbuf[j];
                    if (c == '%')
                    {
                        // get hex val of the next two characters
                        msg[cursor] = 0;
                        for (int k = 1; k <= 2; k++)
                        {
                            c = recvbuf[j + k];
                            if (c >= 'A')
                            {
                                c = c - 'A' + 10;
                            }
                            else
                            {
                                c -= '0';
                            }
                            msg[cursor] <<= 4;
                            msg[cursor] |= c;
                        }
                        j += 2;
                    }
                    else if (c == '+')
                    {
                        msg[cursor] = ' ';
                    }
                    else
                    {
                        msg[cursor] = c;
                    }
                }
                msg[len] = 0; // terminator
                printf("Parsed (%d): %s\n", len, msg);

                // test message
                if (!memcmp(msg, "/quit", 5))
                {
                    printf("Received quit message\n");
                    running = 0;
                }
                free(msg);
            }
        }
        else if (!res)
        {
            printf("Client disconnected\n");
        }
        else
        {
            printf("Receive failed\n");
        }

        // shutdown client
        shutdown(client, SD_BOTH);
        closesocket(client);
        client = INVALID_SOCKET;
    }

    cleanup(listener);
    printf("Shutting down.\nGood night.\n");

    return 0;
}

void cleanup(SOCKET listener)
{
    if (listener && listener != INVALID_SOCKET)
    {
        closesocket(listener);
    }
    WSACleanup();
}

int readFile(const char *filename, char **output)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        return 0;
    }

    // get length
    // move cursor to end
    fseek(fp, 0L, SEEK_END);
    // get remaining length
    int len = ftell(fp);
    // return to original position
    fseek(fp, 0, SEEK_SET);

    // read
    *output = malloc(len + 1);
    fread(*output, len, 1, fp);
    (*output)[len] = 0;
    fclose(fp);

    return len;
}