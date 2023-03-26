// DOWNLOAD
if (strcmp(request, "DOWNLOAD") == 0) {
    char *filename = strtok(NULL, " "); // get the filename from the request
    char filepath[PATH_MAX];
    snprintf(filepath, PATH_MAX, "%s/%s", server_dir, filename); // create the full file path
    FILE *file = fopen(filepath, "rb"); // open the file in binary mode
    if (file == NULL) {
        // send an error message to the client if the file doesn't exist
        char *error_msg = "ERROR File not found\r\n";
        send(client_sockfd, error_msg, strlen(error_msg), 0);
    } else {
        // read the file and send its contents to the client
        char buffer[BUFFER_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
            send(client_sockfd, buffer, bytes_read, 0);
        }
        fclose(file);
    }
}


// DELETE
if (strcmp(request, "DELETE") == 0) {
    char *filename = strtok(NULL, " "); // get the filename from the request
    char filepath[PATH_MAX];
    snprintf(filepath, PATH_MAX, "%s/%s", server_dir, filename); // create the full file path
    if (remove(filepath) == 0) {
        // send a success message to the client if the file was deleted
        char *success_msg = "SUCCESS\r\n";
        send(client_sockfd, success_msg, strlen(success_msg), 0);
    } else {
        // send an error message to the client if the file couldn't be deleted
        char *error_msg = "ERROR File not found\r\n";
        send(client_sockfd, error_msg, strlen(error_msg), 0);
    }
}


//RENAME
if (strcmp(request, "RENAME") == 0) {
    char *old_filename = strtok(NULL, " "); // get the old filename from the request
    char *new_filename = strtok(NULL, "\r\n"); // get the new filename from the request
    char old_filepath[PATH_MAX];
    char new_filepath[PATH_MAX];
    snprintf(old_filepath, PATH_MAX, "%s/%s", server_dir, old_filename); // create the full old file path
    snprintf(new_filepath, PATH_MAX, "%s/%s", server_dir, new_filename); // create the full new file path
    if (rename(old_filepath, new_filepath) == 0) {
        // send a success message to the client if the file was renamed
        char *success_msg = "SUCCESS\r\n";
        send(client_sockfd, success_msg, strlen(success_msg), 0);
    } else {
        // send an error message to the client if the file couldn't be renamed
        char *error_msg = "ERROR File not found\r\n";
        send(client_sockfd, error_msg, strlen(error_msg), 0);
    }
}


//
int server_socket, client_socket;
struct sockaddr_in server_address, client_address;

// create server socket
server_socket = socket(AF_INET, SOCK_STREAM, 0);
if (server_socket < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
}

// bind the socket to a specific port
memset(&server_address, 0, sizeof(server_address));
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = INADDR_ANY;
server_address.sin_port = htons(PORT_NUMBER);

if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
}

// listen for incoming connections
if (listen(server_socket, MAX_PENDING_CONNECTIONS) < 0) {
    perror("listen failed");
    exit(EXIT_FAILURE);
}

//
while (1) {
    // accept incoming connection
    socklen_t client_address_size = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_size);
    if (client_socket < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    // create new thread to handle client request
    pthread_t thread_id;
    int* client_socket_ptr = malloc(sizeof(int));
    *client_socket_ptr = client_socket;
    if (pthread_create(&thread_id, NULL, handle_client_request, (void*)client_socket_ptr) != 0) {
        perror("thread creation failed");
        exit(EXIT_FAILURE);
    }
}

//
void* handle_client_request(void* arg) {
    int client_socket = *((int*)arg);
    free(arg);

    // read client request
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    if (read(client_socket, buffer, BUFFER_SIZE) < 0) {
        perror("read failed");
        close(client_socket);
        pthread_exit(NULL);
    }

    // parse client request
    char* token = strtok(buffer, " ");
    if (token == NULL) {
        // invalid request
        close(client_socket);
        pthread_exit(NULL);
    }

    if (strcmp(token, "DOWNLOAD") == 0) {
        // download file
        token = strtok(NULL, " ");
        if (token == NULL) {
            // invalid request
            close(client_socket);
            pthread_exit(NULL);
        }

        // open file for reading
        FILE* file = fopen(token, "rb");
        if (file == NULL) {
            // file not found
            write(client_socket, "ERROR FILE_NOT_FOUND", strlen("ERROR FILE_NOT_FOUND"));
            close(client_socket);
            pthread_exit(NULL);
        }

        // send file to client
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
            if (write(client_socket,
