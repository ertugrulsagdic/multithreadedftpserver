/* server.c - code for example server program that uses TCP */
#ifndef unix
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#define MAX_CLIENTS 10   /* maximum number of clients */
#define BUFFER_SIZE 1024 /* size of buffer for string the server sends */
#define PROTOPORT 5193   /* default protocol port number */
#define QLEN 6           /* size of request queue */
int visits = 0;          /* counts client connections */

void *handle_client(void *arg);

/*------------------------------------------------------------------------
 * * Program: server
 * *
 * * Purpose: allocate a socket and then repeatedly execute the following:
 * * (1) wait for the next connection from a client
 * * (2) send a short message to the client
 * * (3) close the connection
 * * (4) go back to step (1)
 * * Syntax: server [ port ]
 * *
 * * port - protocol port number to use
 * *
 * * Note: The port argument is optional. If no port is specified,
 * * the server uses the default given by PROTOPORT.
 * *
 * *------------------------------------------------------------------------
 * */
main(argc, argv) int argc;
char *argv[];
{
    struct hostent *ptrh;   /* pointer to a host table entry */
    struct protoent *ptrp;  /* pointer to a protocol table entry */
    struct sockaddr_in sad; /* structure to hold server.s address */
    struct sockaddr_in cad; /* structure to hold client.s address */
    int sd, client_fd;      /* socket descriptors */
    int port;               /* protocol port number */
    int alen;               /* length of address */
    char buf[BUFFER_SIZE];  /* buffer for string the server sends */
    pthread_t tid[MAX_CLIENTS];
    int client_count = 0;
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);
#endif
    memset((char *)&sad, 0, sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;             /* set family to Internet */
    sad.sin_addr.s_addr = INADDR_ANY;     /* set the local IP address */
    /* Check command-line argument for protocol port and extract */
    /* port number if one is specified. Otherwise, use the default */
    /* port value given by constant PROTOPORT */
    if (argc > 1)
    {                         /* if argument specified */
        port = atoi(argv[1]); /* convert argument to binary */
    }
    else
    {
        port = PROTOPORT; /* use default port number */
    }
    if (port > 0) /* test for illegal value */
        sad.sin_port = htons((u_short)port);
    else
    { /* print error message and exit */
        fprintf(stderr, "bad port number %s\n", argv[1]);
        exit(1);
    }
    /* Map TCP transport protocol name to protocol number */
    if (((int)(ptrp = getprotobyname("tcp"))) == 0)
    {
        fprintf(stderr, "cannot map \"tcp\" to protocol number");
        exit(1);
    }
    /* Create a socket */
    sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sd < 0)
    {
        fprintf(stderr, "socket creation failed\n");
        exit(1);
    }
    /* Bind a local address to the socket */
    if (bind(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        fprintf(stderr, "bind failed\n");
        exit(1);
    }
    /* Specify size of request queue */
    if (listen(sd, QLEN) < 0)
    {
        fprintf(stderr, "listen failed\n");
        exit(1);
    }

    fprintf(stdout, "Server is listening...\n");

    /* Main server loop - accept and handle requests */
    while (1)
    {
        alen = sizeof(cad);
        if ((client_fd = accept(sd, (struct sockaddr *)&cad, &alen)) < 0)
        {
            fprintf(stderr, "accept failed\n");
            exit(1);
        }

        if (pthread_create(&tid[client_count], NULL, handle_client, (void *)&client_fd) != 0)
        {
            fprintf(stderr, "Failed to create thread\n");
        }

        client_count++;
    }
}

void *handle_client(void *arg)
{
    int client_fd = *(int *)arg;
    char buffer[BUFFER_SIZE] = {0};
    int valread;

    printf("New client connected: %d\n", client_fd);

    char response[BUFFER_SIZE] = {0};
    sprintf(response, "Connected to server");
    send(client_fd, response, strlen(response), 0);

    while ((valread = read(client_fd, buffer, BUFFER_SIZE)) > 0)
    {
        printf("Client %d sent: %s\n", client_fd, buffer);

        memset(buffer, 0, BUFFER_SIZE);
    }

    printf("Client %d disconnected\n", client_fd);

    closesocket(client_fd);
    pthread_exit(NULL);
}
