#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 1. Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    // 2. Server address setup
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9877); // Change to your server port if needed
    if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) { // Change IP if needed
        std::cerr << "Invalid address\n";
        return 1;
    }

    // 3. Connect to server
    if (connect(sockfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }
    std::cout << "Connected to server!\n";

    // 4. Build CREATE_MEETING message (11 bytes)
    char msg[11];
    msg[0] = '$';

    uint16_t msgType = htons(1); // Suppose CREATE_MEETING is 1
    memcpy(msg + 1, &msgType, 2);

    uint32_t ip = htonl(0); // Not used, set to 0
    memcpy(msg + 3, &ip, 4);

    uint32_t dataSize = htonl(0); // No extra data
    memcpy(msg + 7, &dataSize, 4);

    msg[10] = '#'; // End with #

    ssize_t sent = send(sockfd, msg, 11, 0);
    if (sent < 0) {
        std::cerr << "Send failed\n";
        close(sockfd);
        return 1;
    }
    std::cout << "Sent CREATE_MEETING message to server.\n";

    // 5. Try to receive response from server
    char buf[1024] = {0};
    ssize_t received = recv(sockfd, buf, sizeof(buf)-1, 0);
    if (received > 0) {
        std::cout << "Server response: " << buf << std::endl;
    } else if (received == 0) {
        std::cout << "Server closed connection.\n";
    } else {
        std::cout << "Receive failed.\n";
    }

    // 6. Close socket
    close(sockfd);
    return