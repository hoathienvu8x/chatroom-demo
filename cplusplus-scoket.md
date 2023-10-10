---
title: "How To Create A C++ Scoket And Implement Network Communication"
linke: "https://marketsplash.com/tutorials/cpp/cplusplus-scoket/"
author: "Severin Ilya"
publish: "2023-08-15T19:19:00"
---

In this article, we'll explore the essentials of working with sockets in
C++. From establishing simple client-server connections to handling multiple
clients with multithreading, we'll walk through practical examples that
illuminate the core concepts.

C++ sockets provide a fundamental interface for programming network communication.
They are essential tools for creating server and client applications that
can communicate over the internet. In this article, you'll learn the basics
of working with C++ sockets, from setting up a simple server to crafting
a multi-threaded networking solution.

![](https://marketsplash.com/content/images/size/w1000/2023/08/mermaid-diagram-2023-08-19-010737.png)

## Understanding C++ Sockets

In networking, a socket is a fundamental concept that represents an endpoint
for communication between two machines. In C++, sockets are used to establish
a connection between a client and a server, enabling them to send and receive data.

## Socket Creation

Creating a socket in C++ involves calling the `socket` function. This function
takes three parameters: the domain (usually `AF_INET` for IPv4), the type
(such as `SOCK_STREAM` for TCP), and the protocol (usually `0` to automatically
select the protocol based on the type).

```c++
#include <sys/socket.h>
// ...
int my_socket = socket(AF_INET, SOCK_STREAM, 0);
```

## Socket Types

There are two main types of sockets: stream sockets (TCP) and datagram sockets
(UDP). Stream sockets are reliable and connection-oriented, while datagram
sockets are connectionless and may not guarantee delivery.

```c++
int tcp_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP socket
int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);  // UDP socket
```

## Socket Addressing

To specify where a socket should connect or bind, we use a socket address.
This is a combination of an IP address and a port number, represented in
C++ by the `sockaddr_in` structure.

```c++
#include <netinet/in.h>
// ...
struct sockaddr_in address;
address.sin_family = AF_INET;
address.sin_port = htons(8080);
address.sin_addr.s_addr = INADDR_ANY;
```

## Closing Socket

After communication is complete, it is essential to close the socket. This
is done using the `close` function, which releases the resources associated
with the socket.

```c++
close(my_socket);
```

Understanding C++ sockets is foundational for network programming, as they
enable applications to communicate over a network in a structured and
standardized way.

## Setting Up A Socket Server

To set up a socket server in C++, the first step is to create a socket.
This is done using the `socket` function, which returns a socket descriptor.
The key parameters for this function are the domain (usually `AF_INET`
for IPv4), the type (such as `SOCK_STREAM` for TCP), and the protocol
(usually `0` to automatically select the protocol based on the type).

```c++
#include <sys/socket.h>
// ...
int server_socket = socket(AF_INET, SOCK_STREAM, 0);
```

## Configuring Server Address

The next step is to bind the socket to a specific IP address and port number.
This is done using the `bind` function. Before calling `bind`, we need to
set up a `sockaddr_in` structure that specifies the address and port.

```c++
#include <netinet/in.h>
// ...
struct sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_port = htons(8080); // Port number
server_address.sin_addr.s_addr = INADDR_ANY; // Bind to any local address
```

```c++
bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
```

## Listening For Connections

After binding, the server needs to listen for incoming connections. This
is done using the `listen` function, which takes two parameters: the socket
descriptor and the maximum number of pending connections.

```c++
listen(server_socket, 5);
```

## Accepting A Connection

To accept an incoming connection, use the `accept` function. This function
blocks until a client connects to the server. When a connection is established,
`accept` returns a new socket descriptor for the connection.

```c++
int client_socket = accept(server_socket, NULL, NULL);
```

With these steps, you have set up a basic socket server in C++. The server
is now ready to send and receive data through the `client_socket` descriptor.

## Creating A Socket Client

To establish a socket client in C++, the initial step is similar to that
of a server: creating a socket. This is accomplished using the `socket`
function, which returns a socket descriptor. The parameters for this
function are the domain (usually `AF_INET` for IPv4), the type (such as
`SOCK_STREAM` for TCP), and the protocol (usually `0` to automatically
select the protocol based on the type).

```c++
#include <sys/socket.h>
// ...
int client_socket = socket(AF_INET, SOCK_STREAM, 0);
```

After creating the socket, the client needs to connect to a server. This
is done using the `connect` function. Before calling `connect`, set up a
`sockaddr_in` structure that specifies the server's address and port.

```c++
#include <netinet/in.h>
// ...
struct sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_port = htons(8080); // Server port number
server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP
```

```c++
connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address));
```

Once connected, the client can send and receive data using the `send` and
`recv` functions, respectively. These functions use the socket descriptor
returned by the `socket` function.

```c++
char buffer[1024] = "Hello Server!";
send(client_socket, buffer, sizeof(buffer), 0);
```

```c++
char response[1024];
recv(client_socket, response, sizeof(response), 0);
```

After communication is complete, it is important to close the socket to
free up system resources. This is done using the `close` function.

```c++
close(client_socket);
```

With these steps, you have established a basic socket client in C++. The
client is now capable of connecting to a server, sending messages, receiving
responses, and properly closing the connection when done.

## Handling Multiple Clients

In a real-world scenario, a server often needs to handle multiple clients
simultaneously. One common approach to achieve this is by using multithreading.
Each client connection is handled by a separate thread, allowing the server
to manage multiple clients at the same time.

### Echo Server-Client Application

To implement a simple echo server-client application where the server echoes
the messages sent from the client.

```c++
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int server_socket, client_socket;
    sockaddr_in server_address, client_address;
    socklen_t client_length;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 5);

    std::cout << "Server is listening on port 8080..." << std::endl;

    client_length = sizeof(client_address);
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_length);

    char buffer[1024] = {0};
    read(client_socket, buffer, sizeof(buffer));
    std::cout << "Received: " << buffer << std::endl;

    send(client_socket, buffer, strlen(buffer), 0);

    close(client_socket);
    close(server_socket);

    return 0;
}
```

### Client Side Code:

```c++
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int client_socket;
    sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    char message[1024] = "Hello, Server!";
    send(client_socket, message, strlen(message), 0);

    char buffer[1024] = {0};
    read(client_socket, buffer, sizeof(buffer));
    std::cout << "Server replied: " << buffer << std::endl;

    close(client_socket);

    return 0;
}
```

### Steps to Run:

Firstly, the server code needs to be compiled with the command
`g++ -o server server.cpp` and then started with the command `./server`.

In another terminal, the client code should be compiled using the command
`g++ -o client client.cpp` and run with the command `./client`.

### Expected Outcome:

When executed, the server will be actively waiting for connections on port
8080. The client will then connect, send its message "Hello, Server!",
and receive an echo of this message from the server. The server will show
the received message "Received: Hello, Server!", and the client will
display the echoed message "Server replied: Hello, Server!".

## Creating A Thread For Each Client

When the server accepts a new client connection, it creates a new thread
specifically for that client. This thread is responsible for all communication
with the connected client.

```c++
#include <pthread.h>
// ...
void* handle_client(void* client_socket) {
    // Communication with the client
    // ...
}

int client_socket = accept(server_socket, NULL, NULL);
pthread_t client_thread;
pthread_create(&client_thread, NULL, handle_client, (void*) &client_socket);
```

### Managing Client Data

To keep track of each client, the server might use a data structure (like
an array or a list) that stores information about all connected clients.
This could include the client's socket descriptor, address information,
and any other relevant data.

```c++
#include <vector>
// ...
struct ClientInfo {
    int socket;
    struct sockaddr_in address;
    // Other client-specific data
};

std::vector<ClientInfo> clients;
```

## Handling Client Disconnections

It’s important for the server to detect when a client has disconnected and
to clean up the resources associated with that client. This often involves
closing the client's socket and removing its data from the server's list
of clients.

```c++
close(client_socket);
clients.erase(std::remove_if(clients.begin(), clients.end(),
    [&client_socket](const ClientInfo& client) { return client.socket == client_socket; }),
    clients.end());
```

## Synchronizing Access to Shared Resources

When multiple threads are involved, synchronization is key to prevent race
conditions. For instance, when modifying the list of clients, it’s essential
to ensure that only one thread can modify this list at a time.

```c++
#include <mutex>
// ...
std::mutex client_list_mutex;

// When modifying the client list:
client_list_mutex.lock();
// Modify the client list
client_list_mutex.unlock();
```

With these steps, your server is now capable of handling multiple clients
simultaneously in a thread-safe manner, efficiently managing client data,
and gracefully handling client disconnections.

## Error Handling And Exceptions

In network programming with C++, error handling is a critical aspect.
Functions like `socket`, `bind`, `listen`, `connect`, and `accept` return
specific values when they fail, usually `-1`. It is essential to check
these return values and handle errors gracefully.

```c++
int server_socket = socket(AF_INET, SOCK_STREAM, 0);
if (server_socket == -1) {
    perror("Failed to create socket");
    exit(EXIT_FAILURE);
}
```

When a client attempts to connect to a server, the `connect` function might
fail. It’s important to detect this failure and decide how to proceed,
such as by retrying the connection or reporting an error to the user.

```c++
if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
    perror("Failed to connect to server");
    close(client_socket);
    exit(EXIT_FAILURE);
}
```

When sending or receiving data using the `send` and `recv` functions, errors
can occur, such as a broken connection. It is vital to check for errors
during these operations and handle them appropriately.

```c++
ssize_t sent_bytes = send(client_socket, buffer, sizeof(buffer), 0);
if (sent_bytes == -1) {
    perror("Failed to send data");
    close(client_socket);
    exit(EXIT_FAILURE);
}
```

While C++ socket functions typically report errors via return values, you
can also use C++ exceptions to handle errors in a more structured way.
This involves throwing an exception when an error is detected and catching
it in a higher-level function.

```c++
void connect_to_server(int client_socket, struct sockaddr_in server_address) {
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        throw std::runtime_error("Failed to connect to server");
    }
}

try {
    connect_to_server(client_socket, server_address);
} catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << '\n';
    close(client_socket);
    exit(EXIT_FAILURE);
}
```

With proper error handling and exception management, your C++ network
application becomes more robust and can deal with unexpected situations
in a controlled and predictable manner.
