#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

#include "conf.h"
#include "server.h"
#include "client.h"

int main(int argc, char *argv[])
{
    srand(time(NULL));

    // Create a socket.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    // Create address of the server.
    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    // Try to connect to the server.
    int connection_status = connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1)
    {
        // Create a server.
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        {
            perror("setsockopt(SO_REUSEADDR) failed");
            exit(2);
        }

        // Bind the socket to our specified IP and port.
        int bind_status = bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));
        if (bind_status == -1)
        {
            perror("ERROR on binding");
            exit(3);
        }

        // Listen for connections.
        int listen_status = listen(socket_fd, 20);
        if (listen_status == -1)
        {
            perror("ERROR on listening");
            exit(4);
        }

        server_main_loop(socket_fd);
    }
    else
    {
        // Connect to the server.
        client_main_loop(socket_fd);
    }

    pthread_exit(0);
}
