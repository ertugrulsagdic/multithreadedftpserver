# Multithreaded FTP Server
## Description
This is a multithreaded FTP server program that allows clients to download, upload, delete, and rename files on the server. The server is written in C and uses socket programming to communicate with clients over the network. The server also supports multi-threading, which allows it to handle multiple clients at the same time.

## Requirements
To compile and build the server and client applications, you will need to have GCC (GNU Compiler Collection) installed on your computer. 

You can use the following commands to compile and build the server and client applications:


```console
gcc server.c -o server.c -lpthread
```

```console
gcc client.c -o client
```

## Usage
To run the server, you need to provide a port number as a command line argument. You can do this by typing the following command:

```console
./server <port_number>
```

To connect to the server, you need to run the client program and provide the IP address or localhost machine name and port number as arguments. You can do this by typing the following command:

```console
./client <server_ip_address> <port_number>
```

## Once you have connected to the server, you can perform the following actions:

* 0\) Show all files: Displays a list of all files in the client's current directory.

* 1\) Show all files in the server: Displays a list of all files in the server's * current directory.

* 2\) Download file from server with name: Downloads a file from the server with the specified name to the client's current directory.

* 3\) Upload file to server: Uploads a file from the client's current directory to the server's current directory.

* 4\) Delete file from the server with name: Deletes a file from the server with the specified name.

* 5\) Rename the file in server: Renames a file in the server with the specified name.

* 6\) Exit: Exits the client program.

To select an action, enter the corresponding number (0-6) and press Enter. If you enter an invalid choice, the program will display an error message and prompt you to enter a valid choice.

At some actions the program will ask you to enter a file name. If you enter an invalid file name, the program will display an error message. You have to select the same action to try again with the correct file name.

The server will continue running until it is manually stopped. To stop the server, press Ctrl+C.