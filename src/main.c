//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>

#include "server.h"
//#include "conf.h"
//#include "client.h"

int main(int argc, char *argv[])
{
//    main_loop(1);
    server_main_loop(1);
//    // Create socket.
//    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);  // IPv4, TCP, default protocol.
//
//    // Specify address for the socket.
//    struct sockaddr_in server_address;
//    server_address.sin_family = AF_INET;
//    server_address.sin_port = htons(PORT);
//    server_address.sin_addr.s_addr = INADDR_ANY;
//
//    int connection = connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
//    if (connection == -1)
//    {
//        server_main_loop();
//    }
//    else
//    {
//        printf("Connected to server.\n");
//
//        // Receive data from the server.
//        char server_response[256];
//        recv(socket_fd, &server_response, sizeof(server_response), 0);
//
//        // Print out the server's response.
//        printf("The server sent the data: %s\n", server_response);
//
//        close(socket_fd);
//    }
////    server_main_loop();
    return 0;
}
