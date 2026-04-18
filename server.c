#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    char buffer[1024];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);

    char method[10], path[100];

    sscanf(buffer, "%s %s", method, path);

    // Decide which file to serve

    char filename[100];
    if (strcmp(path, "/") == 0) {
        strcpy(filename, "index.html");
    } else {
        strcpy(filename, path + 1);
    }

    // Open the requested file

    FILE *file = fopen(filename, "r");
    while (1) {
    client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    printf("Client connected!\n");

    memset(buffer, 0, 1024);
    read(client_socket, buffer, 1024);
    printf("Request:\n%s\n", buffer);

    char method[10], path[100];
    sscanf(buffer, "%s %s", method, path);

    // Decide file
    char filename[100];

    if (strcmp(path, "/") == 0) {
        strcpy(filename, "index.html");
    } else {
        strcpy(filename, path + 1);
    }

    // Open file
    FILE *file = fopen(filename, "r");

    if (file != NULL) {
        char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(client_socket, header, strlen(header), 0);

        char file_buffer[1024];
        int bytes;

        while ((bytes = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
            send(client_socket, file_buffer, bytes, 0);
        }

        fclose(file);
    } else {
        char *not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n\r\n"
            "<html><body><h1>404 Not Found</h1></body></html>";

        send(client_socket, not_found, strlen(not_found), 0);
    }

    close(client_socket);
}

    close(server_fd);
    return 0;
}