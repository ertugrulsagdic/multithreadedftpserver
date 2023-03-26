/* client.c - code for example client program that uses TCP */
#ifndef unix
#define WIN32
#include <windows.h>
#include <winsock.h>
#else
#define closesocket close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <string.h>
#define PROTOPORT 5193 /* default protocol port number */
#define MAX_LENGTH 1024 /* maximum buffer size */
extern int errno;
char localhost[] = "localhost"; /* default host name */


void show_file_names();
void upload_file();
void download_file();
void delete_file();
void rename_file();

/*------------------------------------------------------------------------
 * * Program: client
 * *
 * * Purpose: allocate a socket, connect to a server, and print all output
 * * Syntax: client [ host [port] ]
 * *
 * * host - name of a computer on which server is executing
 * * port - protocol port number server is using
 * *
 * * Note: Both arguments are optional. If no host name is specified,
 * * the client uses "localhost"; if no protocol port is
 * * specified, the client uses the default given by PROTOPORT.
 * *
 * *------------------------------------------------------------------------
 * */
main(argc, argv) int argc;
char *argv[];
{
    struct hostent *ptrh;   /* pointer to a host table entry */
    struct protoent *ptrp;  /* pointer to a protocol table entry */
    struct sockaddr_in sad; /* structure to hold an IP address */
    int sd;                 /* socket descriptor */
    int port;               /* protocol port number */
    char *host;             /* pointer to host name */
    int n;                  /* number of characters read */
    char buf[1000];         /* buffer for data from the server */
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(0x0101, &wsaData);
#endif
    memset((char *)&sad, 0, sizeof(sad)); /* clear sockaddr structure */
    sad.sin_family = AF_INET;             /* set family to Internet */
    /* Check command-line argument for protocol port and extract */
    /* port number if one is specified. Otherwise, use the default */
    /* port value given by constant PROTOPORT */
    if (argc > 2)
    {                         /* if protocol port specified */
        port = atoi(argv[2]); /* convert to binary */
    }
    else
    {
        port = PROTOPORT; /* use default port number */
    }
    if (port > 0) /* test for legal value */
        sad.sin_port = htons((u_short)port);
    else
    { /* print error message and exit */
        fprintf(stderr, "bad port number %s\n", argv[2]);
        exit(1);
    }
    /* Check host argument and assign host name. */
    if (argc > 1)
    {
        host = argv[1]; /* if host argument specified */
    }
    else
    {
        host = localhost;
    }
    /* Convert host name to equivalent IP address and copy to sad. */
    ptrh = gethostbyname(host);
    if (((char *)ptrh) == NULL)
    {
        fprintf(stderr, "invalid host: %s\n", host);
        exit(1);
    }
    memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
    /* Map TCP transport protocol name to protocol number. */
    if (((int)(ptrp = getprotobyname("tcp"))) == 0)
    {
        fprintf(stderr, "cannot map \"tcp\" to protocol number");
        exit(1);
    }
    /* Create a socket. */
    sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sd < 0)
    {
        fprintf(stderr, "socket creation failed\n");
        exit(1);
    }
    /* Connect the socket to the specified server. */
    if (connect(sd, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        fprintf(stderr, "connect failed\n");
        exit(1);
    }

    // // Send command to server
    // char command[MAX_LENGTH];
    // strcpy(command, argv[3]);
    // ssize_t request = send(sd, command, strlen(command), 0);
    // if (request < 0) {
    //     fprintf(stderr, "error sending command\n");
    //     exit(1);
    // }

    /* Repeatedly read data from socket and write to user.s screen. */
    char response[MAX_LENGTH];
    memset(response, 0, MAX_LENGTH);
    n = recv(sd, response, MAX_LENGTH, 0);
    if (n < 0) {
        fprintf(stderr, "error receiving response\n");
        exit(1);
    }

    printf("%s\n", response);
    
    int choice;

    while (1) {
        printf("Please select an action:\n");
        printf("1. Show all files in the server\n");
        printf("2. Download file from server with name\n");
        printf("3. Upload file to server\n");
        printf("4. Delete file from the server with name\n");
        printf("5. Rename the file in server\n");
        printf("6. Exit\n");
        printf("Enter your choice (1-6): ");

        if (scanf("%d", &choice) != 1) {
            // Clear input buffer if non-integer input is entered
            while (getchar() != '\n');
            printf("Invalid choice!\n");
            continue;
        }

        switch (choice) {
            case 1:
                show_file_names();
                break;
            case 2:
                download_file();
                break;
            case 3:
                upload_file();
                break;
            case 4:
                delete_file();
                break;
            case 5:
                rename_file();
                break;

            case 6:
                /* Close the socket. */
                closesocket(sd);
                /* Terminate the client program gracefully. */
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }
}

void show_file_names() {
    // Code to show all the files in the server
    printf("Showing all files in the server...\n");
}

void download_file() {
    char filename[MAX_LENGTH];

    printf("Enter the filename to download: ");
    scanf("%s", filename);

    // Code to download the file from the server
    printf("Downloading file '%s' from server...\n", filename);
}

void upload_file() {
    char filename[MAX_LENGTH];

    printf("Enter the filename to upload: ");
    scanf("%s", filename);

    // Code to upload the file to the server
    printf("Uploading file '%s' to server...\n", filename);
}

void delete_file() {
    char filename[MAX_LENGTH];

    printf("Enter the filename to delete: ");
    scanf("%s", filename);

    // Code to delete the file from the server
    printf("Deleting file '%s' from server...\n", filename);
}

void rename_file() {
    char old_filename[MAX_LENGTH];
    char new_filename[MAX_LENGTH];

    printf("Enter the old filename: ");
    scanf("%s", old_filename);

    printf("Enter the new filename: ");
    scanf("%s", new_filename);

    // Code to rename the file in the server
    printf("Renaming file '%s' to '%s'...\n", old_filename, new_filename);
}