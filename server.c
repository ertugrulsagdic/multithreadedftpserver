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
#include <dirent.h>
#define MAX_CLIENTS 10   /* maximum number of clients */
#define BUFFER_SIZE 1024 /* size of buffer for string the server sends */
#define PROTOPORT 5193   /* default protocol port number */
#define QLEN 6           /* size of request queue */
int visits = 0;          /* counts client connections */

void *handle_client(void *arg);

void show_file_names(int client_fd);
void upload_file(int client_fd);
void download_file(int client_fd);
void delete_file(int client_fd);
void rename_file(int client_fd);

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
    // remove all files in the server directory except the download.txt file
    DIR *server_dir;
    struct dirent *dir;
    server_dir = opendir("./server_files");
    if (server_dir)
    {
        while ((dir = readdir(server_dir)) != NULL)
        {
            if (strcmp(dir->d_name, "download.txt") != 0)
            {
                char filepath[BUFFER_SIZE];
                snprintf(filepath, BUFFER_SIZE, "%s/%s", "server_files", dir->d_name);
                remove(filepath);
                bzero(filepath, BUFFER_SIZE);
            }
        }
        closedir(server_dir);
    }

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

        int choice = atoi(buffer);

        switch (choice)
        {
        case 1:
            show_file_names(client_fd);
            break;
        case 2:
            download_file(client_fd);
            break;
        case 3:
            upload_file(client_fd);
            break;
        case 4:
            delete_file(client_fd);
            break;
        case 5:
            rename_file(client_fd);
            break;
            exit(0);
        default:
            printf("Invalid choice!\n");
        }

        memset(buffer, 0, BUFFER_SIZE);
    }

    printf("Client %d disconnected\n", client_fd);

    closesocket(client_fd);
    pthread_exit(NULL);
}

void show_file_names(int client_fd)
{
    // Get all file names in the current directory
    DIR *d;
    struct dirent *dir;
    d = opendir("./server_files");
    int count = 0;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char *filename = dir->d_name;
            if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
                continue;
            send(client_fd, filename, strlen(filename), 0);
            send(client_fd, "\n", 1, 0);
            count++;
        }
        closedir(d);
    }
    printf("Total files: %d\n", count);
    if (count == 0)
    {
        char *msg = "No files found in server\n";
        send(client_fd, msg, strlen(msg), 0);
    }

    char *ack = "END";
    send(client_fd, ack, strlen(ack), 0);
    printf("File names sent to client %d\n", client_fd);
}

void download_file(int client_fd)
{
    // read filename from client
    char buffer[BUFFER_SIZE];
    int valread = read(client_fd, buffer, BUFFER_SIZE);
    if (valread < 0)
    {
        printf("Error reading from client\n");
        return;
    }

    char *filename = buffer;

    printf("Filename to download: %s\n", filename);

    char filepath[BUFFER_SIZE];
    snprintf(filepath, BUFFER_SIZE, "%s/%s", "server_files", filename); // create the full file path
    FILE *file = fopen(filepath, "rb");                                 // open the file in binary mode
    if (file == NULL)
    {
        // send an error message to the client if the file doesn't exist
        char *error_msg = "ERROR: File not found\n";
        send(client_fd, error_msg, strlen(error_msg), 0);
    }
    else
    {
        // read the file and send its contents to the client
        char buffer[BUFFER_SIZE];
        size_t bytes_read;
        //  Buraya dikkat buyuk boyuttaki file gonderirken farkli calisiyor
        while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0)
        {
            send(client_fd, buffer, bytes_read, 0);
        }
        fclose(file);
    }

    memset(filepath, 0, BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);
}

void upload_file(int client_fd)
{

    char filename[BUFFER_SIZE] = {0};

    int msg = recv(client_fd, filename, BUFFER_SIZE, 0);
    if (msg < 0)
    {
        fprintf(stderr, "error receiving response\n");
        return;
    }

    // check if filename is "ERROR: File not found\r\n"
    if (strcmp(filename, "ERROR: File not found\r\n") == 0)
    {
        return;
    }

    printf("Filename to upload: %s\n", filename);
        printf("len: %d\n", strlen(filename));

    // send ack to client
    send(client_fd, filename, strlen(filename), 0);

    char response[BUFFER_SIZE] = {0};
    int n = recv(client_fd, response, BUFFER_SIZE, 0);
    if (n < 0)
    {
        fprintf(stderr, "error receiving response\n");
        exit(1);
    }

    if (strcmp(response, "ERROR: Filename not received\r\n") == 0)
    {
        printf("Filename not received\n");
        return;
    }
    else
    {
        
        char filepath[BUFFER_SIZE];
        snprintf(filepath, BUFFER_SIZE, "%s/%s", "server_files", filename);
        // create file

        printf("Filepath: %s\n", filepath);


        FILE *fp;
        fp = fopen(filepath, "w");

        if (fp == NULL)
        {
            char *error_msg = "ERROR: something went wrong while creating file on server side\n";
            send(client_fd, error_msg, strlen(error_msg), 0);
            return;
        }

        memset(filepath, '\0', strlen(filepath));
        
        // do while loop to receive file
        do
        {
            fprintf(fp, "%s", response);
            bzero(response, BUFFER_SIZE);
            if (n < BUFFER_SIZE)
            {
                char *success_msg = "File uploaded successfully!\n";
                send(client_fd, success_msg, strlen(success_msg), 0);
                break;
            }
        } while ((n = recv(client_fd, response, BUFFER_SIZE, 0)) > 0);

        fclose(fp);
    }

    memset(response, 0, BUFFER_SIZE);
}

void delete_file(int client_fd)
{
    // read filename from client
    char buffer[BUFFER_SIZE];
    int valread = read(client_fd, buffer, BUFFER_SIZE);
    if (valread < 0)
    {
        printf("Error reading from client\n");
        return;
    }

    char *filename = buffer;

    printf("Filename: %s\n", filename);

    char filepath[BUFFER_SIZE];
    snprintf(filepath, BUFFER_SIZE, "%s/%s", "server_files", filename);

    if (remove(filepath) == 0)
    {
        printf("File deleted successfully\n");
        char *success_msg = "SUCCESS: File deleted\r\n";
        send(client_fd, success_msg, strlen(success_msg), 0);
    }
    else
    {
        printf("Unable to delete file\n");
        char *error_msg = "ERROR: File not deleted\r\n";
        send(client_fd, error_msg, strlen(error_msg), 0);
    }

    memset(filepath, 0, BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);
}

void rename_file(int client_fd)
{
    // read filename from client
    char buffer[BUFFER_SIZE];
    int valread = read(client_fd, buffer, BUFFER_SIZE);
    if (valread < 0)
    {
        printf("Error reading from client\n");
        return;
    }

    char *token = strtok(buffer, ":");
    if (token == NULL)
    {
        printf("ERROR: Missing arguments!\n");
        return;
    }
    char *filename = token;

    token = strtok(NULL, ":");
    char *newFileName = token;

    printf("File Name: %s\n", filename);
    printf("New File Name: %s\n", newFileName);

    char oldFilePath[BUFFER_SIZE];
    char newFilepath[BUFFER_SIZE];
    snprintf(oldFilePath, BUFFER_SIZE, "%s/%s", "server_files", filename);
    snprintf(newFilepath, BUFFER_SIZE, "%s/%s", "server_files", newFileName);

    if (rename(oldFilePath, newFilepath) == 0)
    {
        printf("%s", "File name changed successfully\n");
        char *success_msg = "SUCCESS: File name changed\r\n";
        send(client_fd, success_msg, strlen(success_msg), 0);
    }
    else
    {
        printf("Unable to rename the file\n");
        char *error_msg = "ERROR: File not renamed\r\n";
        send(client_fd, error_msg, strlen(error_msg), 0);
    }

    memset(oldFilePath, 0, BUFFER_SIZE);
    memset(newFilepath, 0, BUFFER_SIZE);
    memset(buffer, 0, BUFFER_SIZE);
}