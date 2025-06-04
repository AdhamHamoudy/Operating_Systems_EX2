// File: src/uds_example.cpp
// Description: Stage 5 - UDS support (stream + datagram) with readiness to receive messages
// Usage: ./uds_example --stream-path /tmp/mysocket_stream --datagram-path /tmp/mysocket_dgram

#include <iostream>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>

struct UDSConfig {
    std::string stream_path;
    std::string datagram_path;
};

void printUsage(const char* progname) {
    std::cerr << "Usage: " << progname
              << " [--stream-path <path>] [--datagram-path <path>]"
              << std::endl;
}

int main(int argc, char* argv[]) {
    UDSConfig config;

    const struct option long_options[] = {
        {"stream-path", required_argument, nullptr, 's'},
        {"datagram-path", required_argument, nullptr, 'd'},
        {nullptr, 0, nullptr, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "s:d:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 's':
                config.stream_path = optarg;
                break;
            case 'd':
                config.datagram_path = optarg;
                break;
            default:
                printUsage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (!config.stream_path.empty() && !config.datagram_path.empty() && config.stream_path == config.datagram_path) {
        std::cerr << "[!] Error: stream and datagram paths must be different.\n";
        return EXIT_FAILURE;
    }

    std::cout << "UDS Configuration Loaded:" << std::endl;
    if (!config.stream_path.empty()) std::cout << "  Stream Path   : " << config.stream_path << std::endl;
    if (!config.datagram_path.empty()) std::cout << "  Datagram Path : " << config.datagram_path << std::endl;

    // Setup and receive on stream socket
    if (!config.stream_path.empty()) {
        int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("socket (stream)");
            return EXIT_FAILURE;
        }

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, config.stream_path.c_str(), sizeof(addr.sun_path) - 1);
        unlink(config.stream_path.c_str());

        if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind (stream)");
            return EXIT_FAILURE;
        }

        if (listen(sockfd, 5) < 0) {
            perror("listen (stream)");
            return EXIT_FAILURE;
        }

        std::cout << "[+] Stream socket ready at: " << config.stream_path << std::endl;

        // Accept a connection (one for demo purposes)
        int client_fd = accept(sockfd, nullptr, nullptr);
        if (client_fd >= 0) {
            char buf[256];
            ssize_t len = read(client_fd, buf, sizeof(buf) - 1);
            if (len > 0) {
                buf[len] = '\0';
                std::cout << "[stream] Received: " << buf << std::endl;
            }
            close(client_fd);
        }
        close(sockfd);
    }

    // Setup and receive on datagram socket
    if (!config.datagram_path.empty()) {
        int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            perror("socket (datagram)");
            return EXIT_FAILURE;
        }

        sockaddr_un addr{};
        addr.sun_family = AF_UNIX;
        std::strncpy(addr.sun_path, config.datagram_path.c_str(), sizeof(addr.sun_path) - 1);
        unlink(config.datagram_path.c_str());

        if (bind(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            perror("bind (datagram)");
            return EXIT_FAILURE;
        }

        std::cout << "[+] Datagram socket ready at: " << config.datagram_path << std::endl;

        // Receive one message (for demo)
        char buf[256];
        sockaddr_un client_addr;
        socklen_t client_len = sizeof(client_addr);
        ssize_t len = recvfrom(sockfd, buf, sizeof(buf) - 1, 0, (sockaddr*)&client_addr, &client_len);
        if (len > 0) {
            buf[len] = '\0';
            std::cout << "[datagram] Received: " << buf << std::endl;
        }
        close(sockfd);
    }

    return 0;
}