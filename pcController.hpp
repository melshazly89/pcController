#ifndef PCCONTROLLER
#define PCCONTROLLER
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <queue>
#include <array>

enum class CMDT: int
{
    CLS_SOCKET = 0,
    USER_CMD   = 1, 
    RESERVED   = 2
}; 

namespace pcController
{
    class pcController
    {
        public:
            /*Constructor Declaration*/
            pcController();
            /*Destructor Declaration*/
            ~pcController();
            void pcTransmit(const std::string& str);
            void pcReceive();
            void close_socket();
            CMDT cmdHandler();
        private:
            int serverSocket;
            int clientSocket;
            static int send_count;
            static int recv_count;
            std::queue<std::string> commands;
    };
}
#endif