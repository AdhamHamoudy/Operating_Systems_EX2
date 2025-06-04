// File: src/drinks_bar.cpp
// Description: Stage 4 - Option parsing with getopt for startup configuration + alarm timeout
// Usage: ./drinks_bar -T <tcp_port> -U <udp_port> [-o <oxygen>] [-c <carbon>] [-h <hydrogen>] [-t <timeout>]

#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <csignal>
#include <unistd.h>

struct Config {
    int tcp_port = -1;
    int udp_port = -1;
    unsigned int oxygen = 0;
    unsigned int carbon = 0;
    unsigned int hydrogen = 0;
    int timeout = 0; // in seconds
};

void printUsage(const char* progname) {
    std::cerr << "Usage: " << progname
              << " -T <tcp_port> -U <udp_port> [--oxygen <n>] [--carbon <n>] [--hydrogen <n>] [--timeout <sec>]"
              << std::endl;
}

// Handler for SIGALRM
void handle_alarm(int sig) {
    (void)sig; // unused
    std::cerr << "\n[!] Timeout reached. Exiting...\n";
    exit(1);
}

int main(int argc, char* argv[]) {
    Config config;

    const struct option long_options[] = {
        {"oxygen", required_argument, nullptr, 'o'},
        {"carbon", required_argument, nullptr, 'c'},
        {"hydrogen", required_argument, nullptr, 'h'},
        {"timeout", required_argument, nullptr, 't'},
        {"tcp-port", required_argument, nullptr, 'T'},
        {"udp-port", required_argument, nullptr, 'U'},
        {nullptr, 0, nullptr, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "o:c:h:t:T:U:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'o':
                config.oxygen = std::atoi(optarg);
                break;
            case 'c':
                config.carbon = std::atoi(optarg);
                break;
            case 'h':
                config.hydrogen = std::atoi(optarg);
                break;
            case 't':
                config.timeout = std::atoi(optarg);
                break;
            case 'T':
                config.tcp_port = std::atoi(optarg);
                break;
            case 'U':
                config.udp_port = std::atoi(optarg);
                break;
            default:
                printUsage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Check required options
    if (config.tcp_port <= 0 || config.udp_port <= 0) {
        std::cerr << "Error: TCP and UDP ports are required." << std::endl;
        printUsage(argv[0]);
        return EXIT_FAILURE;
    }

    // Set timeout alarm if requested
    if (config.timeout > 0) {
        signal(SIGALRM, handle_alarm);
        alarm(config.timeout);
        std::cout << "[*] Timeout set to " << config.timeout << " seconds.\n";
    }

    // Display config
    std::cout << "Configuration Loaded:\n";
    std::cout << "TCP Port     : " << config.tcp_port << std::endl;
    std::cout << "UDP Port     : " << config.udp_port << std::endl;
    std::cout << "Oxygen       : " << config.oxygen << std::endl;
    std::cout << "Carbon       : " << config.carbon << std::endl;
    std::cout << "Hydrogen     : " << config.hydrogen << std::endl;
    std::cout << "Timeout (sec): " << config.timeout << std::endl;

    // TODO: Use the config values in inventory, communication, etc.

    return 0;
}
