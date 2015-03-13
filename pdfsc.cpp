#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <ctime>

#include "pdfs.h"
#include "Cluster.h"
#include "TCPNetworkInterface.h"
#include "UDPNetworkInterface.h"

using namespace pdfs;

int checkArgv(std::string line);

int main(int argc, char**argv)
{
    Cluster cluster;
    //NetworkInterface *nfi = new TCPNetworkInterface(cluster);
    NetworkInterface *nfi = new NETWORK_INTERFACE(cluster);
    if (nfi->connect() == -1)
    {
        std::cout << "No Resource Founded ... ";
        return 0;
    }

    std::string line, cmd, arg;
    while (true) {
        std::cout << ">> ";
        std::getline(std::cin, line);
        std::istringstream iss(line);
        iss >> cmd;
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        // Get start time
        struct timeval t_start, t_end;
        gettimeofday(&t_start, NULL);
        long start_ut = 
            ((long)t_start.tv_sec) * 1000 + (long)t_start.tv_usec / 1000;

        // Execute Command
        if (line == "")
            continue;
        else if (cmd == "exit" || cmd == "quit") {
            std::cout << "Goodbye...\n";
            break;
        }
        else if (cmd == "push") {
            std::string filename;
            iss >> filename;
            nfi->pushFile(filename);
        }
        else if (cmd == "pull") {
            std::string filename;
            iss >> filename;
            nfi->pullFile(filename);
        }
        else if (cmd == "ls") {
            (nfi->getCluster()).printClusterMap();
        }
        else {
            std::cout << "Command not supported now.\n";
        }
        line = "";

        // Get end time, and publish it
        gettimeofday(&t_end, NULL);
        long end_ut = ((long)t_end.tv_sec) * 1000 + (long)t_end.tv_usec / 1000;
        std::cout << "Time elapsed(ms):" << end_ut - start_ut << std::endl;;

    }
    delete nfi;
}

// Arguments check to avoid too long codes in main function
int checkArgv(std::string line)
{
    std::istringstream iss(line);  
    std::string cmd;
    iss >> cmd;
    if (cmd == "push"){
    
    }
    else if (cmd == "pull") {
    }
    else {
    }
    return 0;
}
