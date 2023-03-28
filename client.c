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
#include <dirent.h>
#define PROTOPORT 5193  /* default protocol port number */
#define MAX_LENGTH 1024 /* maximum buffer size */
extern int errno;
char localhost[] = "localhost"; /* default host name */

void show_file_names_client();
void show_file_names(int sd);
void upload_file(int sd);
void download_file(int sd);
void delete_file(int sd);
void rename_file(int sd);

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

    // remove all files in the client directory except the upload.txt file
    DIR *client_dir;
    struct dirent *dir;
    client_dir = opendir("./client_files");
    if (client_dir)
    {
        while ((dir = readdir(client_dir)) != NULL)
        {
            if (strcmp(dir->d_name, "upload.txt") != 0)
            {
                char filepath[MAX_LENGTH];
                snprintf(filepath, MAX_LENGTH, "%s/%s", "client_files", dir->d_name);
                remove(filepath);
                bzero(filepath, MAX_LENGTH);
            }
        }
        closedir(client_dir);
    }

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

    /* Repeatedly read data from socket and write to user.s screen. */
    char response[MAX_LENGTH];
    memset(response, 0, MAX_LENGTH);
    n = recv(sd, response, MAX_LENGTH, 0);
    if (n < 0)
    {
        fprintf(stderr, "error receiving response\n");
        exit(1);
    }

    printf("%s\n", response);

    int choice;

    while (1)
    {
        printf("Please select an action:\n");
        printf("0. Show all files\n");
        printf("1. Show all files in the server\n");
        printf("2. Download file from server with name\n");
        printf("3. Upload file to server\n");
        printf("4. Delete file from the server with name\n");
        printf("5. Rename the file in server\n");
        printf("6. Exit\n");
        printf("Enter your choice (0-6): ");

        if (scanf("%d", &choice) != 1)
        {
            // Clear input buffer if non-integer input is entered
            while (getchar() != '\n')
                ;
            printf("Invalid choice!\n");
            continue;
        }

        switch (choice)
        {
        case 0:
            show_file_names_client();
            break;
        case 1:
            show_file_names(sd);
            break;
        case 2:
            download_file(sd);
            break;
        case 3:
            upload_file(sd);
            break;
        case 4:
            delete_file(sd);
            break;
        case 5:
            rename_file(sd);
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

    printf("Loop exited\n");
}

void show_file_names_client()
{
    printf("\n");
    // Get all file names in the current directory
    DIR *d;
    struct dirent *dir;
    d = opendir("./client_files");
    int count = 0;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char *filename = dir->d_name;
            if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
                continue;
            printf("%s\n", filename);
            count++;
        }
        closedir(d);
    }
    printf("Total files: %d\n", count);
    printf("\n");
}

void show_file_names(int sd)
{
    // Code to show all the files in the server
    printf("Showing all files in the server...\n");

    // Send command to server
    char *command = "1";
    ssize_t request = send(sd, command, strlen(command), 0);
    if (request < 0)
    {
        fprintf(stderr, "error sending command\n");
        exit(1);
    }

    char response[MAX_LENGTH] = {0};

    int valread;

    char *ack = "END";

    while ((valread = recv(sd, response, MAX_LENGTH, 0)) > 0)
    {
        if (strcmp(response, ack) == 0)
        {
            break;
        }
        printf("%s", response);
        bzero(response, MAX_LENGTH);
    }

    printf("Done showing all files in the server\n");
    printf("\n");
}

void download_file(int sd)
{
    printf("\n");
    show_file_names(sd);
    // Send command to server
    char *command = "2";
    ssize_t request = send(sd, command, strlen(command), 0);
    if (request < 0)
    {
        fprintf(stderr, "error sending command\n");
        exit(1);
    }
    char filename[MAX_LENGTH];

    printf("Enter the filename to download: ");
    scanf("%s", filename);

    // send filename to server
    request = send(sd, filename, strlen(filename), 0);
    if (request < 0)
    {
        fprintf(stderr, "error sending filename\n");
        exit(1);
    }

    char response[MAX_LENGTH] = {0};
    int n = recv(sd, response, MAX_LENGTH, 0);
    if (n < 0)
    {
        fprintf(stderr, "error receiving response\n");
        exit(1);
    }

    if (strcmp(response, "ERROR: File not found\n") == 0)
    {
        printf("%s\n", response);
        return;
    }
    else
    {
        char filepath[MAX_LENGTH];
        snprintf(filepath, MAX_LENGTH, "%s/%s", "client_files", filename);
        // create file
        FILE *fp;
        fp = fopen(filepath, "w");
        memset(filepath, '\0', strlen(filepath));

        if (fp == NULL)
        {
            printf("ERROR: File not created!\n\n");
            return;
        }

        // do while loop to receive file
        do
        {
            fprintf(fp, "%s", response);
            bzero(response, MAX_LENGTH);
            if (n < MAX_LENGTH)
            {
                printf("File downloaded successfully!\n\n");
                break;
            }
        } while ((n = recv(sd, response, MAX_LENGTH, 0)) > 0);

        fclose(fp);
    }

    memset(filename, 0, MAX_LENGTH);
}

void upload_file(int sd)
{
    printf("\nUploadable Files:");
    show_file_names_client();
    // Send command to server
    char *command = "3";
    ssize_t request = send(sd, command, strlen(command), 0);
    if (request < 0)
    {
        fprintf(stderr, "error sending command\n");
        exit(1);
    }
    char filename[MAX_LENGTH] = {0};

    printf("Enter the filename to upload: ");
    scanf("%s", filename);

    char filepath[MAX_LENGTH] = {0};
    snprintf(filepath, MAX_LENGTH, "%s/%s", "client_files", filename); // create the full file path
    FILE *file = fopen(filepath, "rb");                                // open the file in binary mode
    if (file == NULL)
    {
        // send an error message to the server if the file doesn't exist
        char *error_msg = "ERROR: File not found\r\n";
        printf("%s\n", error_msg);
        request = send(sd, error_msg, strlen(error_msg), 0);
        return;
    }
    else
    {
        // send filename to server
        request = send(sd, filename, strlen(filename), 0);
        if (request < 0)
        {
            fprintf(stderr, "error sending filename\n");
            return;
        }

        // receive ack from server to indicate that the filename has been received
        char response[MAX_LENGTH] = {0};
        int n = recv(sd, response, MAX_LENGTH, 0);
        if (n < 0)
        {
            fprintf(stderr, "error receiving response\n");
            return;
        }

        if (strcmp(response, filename) == 0)
        {
            // read the file and send its contents to the client
            char buffer[MAX_LENGTH];
            size_t bytes_read;
            //  Buraya dikkat buyuk boyuttaki file gonderirken farkli calisiyor
            while ((bytes_read = fread(buffer, sizeof(char), MAX_LENGTH, file)) > 0)
            {
                send(sd, buffer, bytes_read, 0);
            }
            fclose(file);

            bzero(buffer, MAX_LENGTH);
        }
        else
        {
            char *error_msg = "ERROR: Filename not received\r\n";
            printf("%s\n", error_msg);
            send(sd, error_msg, strlen(error_msg), 0);
            return;
        }
    }

    // receive a message from the server to indicate that the file has been uploaded
    char response[MAX_LENGTH];
    int n = recv(sd, response, MAX_LENGTH, 0);
    if (n < 0)
    {
        fprintf(stderr, "error receiving response\n");
        exit(1);
    }
    printf("%s\n", response);
}

void delete_file(int sd)
{
    printf("\n");
    show_file_names(sd);

    // Send command to server
    char *command = "4";
    ssize_t request = send(sd, command, strlen(command), 0);
    if (request < 0)
    {
        fprintf(stderr, "error sending command\n");
        exit(1);
    }

    char filename[MAX_LENGTH];
    printf("Enter the filename to delete: ");
    scanf("%s", filename);

    // send filename to server
    request = send(sd, filename, strlen(filename), 0);
    if (request < 0)
    {
        fprintf(stderr, "error sending filename\n");
        exit(1);
    }

    // Code to delete the file from the server
    printf("Deleting file '%s' from server...\n", filename);

    char response[MAX_LENGTH];
    int n = recv(sd, response, MAX_LENGTH, 0);
    if (n < 0)
    {
        fprintf(stderr, "error receiving response\n");
        exit(1);
    }
    printf("Server response: %s\n", response);

    memset(filename, 0, MAX_LENGTH);
}

void rename_file(int sd)
{
    printf("\n");
    show_file_names(sd);

    char old_filename[MAX_LENGTH];
    char new_filename[MAX_LENGTH];
    char filenames[MAX_LENGTH];

    printf("\n");
    // Send command to server
    char *command = "5";
    ssize_t request = send(sd, command, strlen(command), 0);
    if (request < 0)
    {
        fprintf(stderr, "error sending command\n");
        exit(1);
    }

    printf("Enter the old filename: ");
    scanf("%s", old_filename);

    printf("Enter the new filename: ");
    scanf("%s", new_filename);

    strcpy(filenames, old_filename);
    strcat(filenames, ":");
    strcat(filenames, new_filename);

    // send filenames to server
    request = send(sd, filenames, strlen(filenames), 0);
    if (request < 0)
    {
        fprintf(stderr, "error sending filename\n");
        exit(1);
    }

    // Code to rename the file in the server
    printf("Renaming file '%s' to '%s'...\n", old_filename, new_filename);

    char response[MAX_LENGTH];
    int n = recv(sd, response, MAX_LENGTH, 0);
    if (n < 0)
    {
        fprintf(stderr, "error receiving response\n");
        exit(1);
    }
    printf("Server response: %s\n", response);

    memset(old_filename, 0, MAX_LENGTH);
    memset(new_filename, 0, MAX_LENGTH);
    memset(filenames, 0, MAX_LENGTH);
}