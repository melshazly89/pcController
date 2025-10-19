#include "pcController.hpp"

/*Alias*/
namespace rc = pcController;

/*Global variables*/
int rc::pcController::send_count = 0;
int rc::pcController::recv_count = 0;
/*Constructor Definition*/
rc::pcController::pcController()
{
    /*Create a socket*/
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
        if(serverSocket == -1)
        {
            std::cerr << "Could not create socket\n";
        }
        // Forcefully attach socket to the port
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

        // specifying the address
        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(8080);
        serverAddress.sin_addr.s_addr = INADDR_ANY;

        // binding socket.
        try
        {
            bind(serverSocket, (struct sockaddr*)&serverAddress,
            sizeof(serverAddress));
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        // listening to the assigned socket
        listen(serverSocket, 100);
        std::cout << "Waiting for incoming connections...\n";
    
        // accepting connection request
        clientSocket = accept(serverSocket, nullptr, nullptr);
        if(clientSocket < 0)
        {
            std::cerr << "Accept failed" << std::endl;
        }
        std::cout << "Connection accepted" << std::endl;
}
/*Destructor Definition*/
rc::pcController::~pcController()
{
    /*DO NOTHING*/
}
void rc::pcController::pcTransmit(const std::string& msg)
{
    try
    {
        send(clientSocket,msg.c_str(),msg.size(),0);

    }
    catch(const std::exception& e)
    {
            std::cerr << e.what() << '\n';
    }
    /*Successfull Transmission */
    send_count++;
    std::cout << "Tx counts : " << send_count << std::endl;
    
}
void rc::pcController::pcReceive()
{
    char buff[1024];
    ssize_t bytesReceived = recv(clientSocket, buff, sizeof(buff) - 1, 0);
    if (0 < bytesReceived)
        {
            /*Add Null Terminator*/
            buff[bytesReceived] = '\0';
            /*Successfull Receive*/
            recv_count++;
            std::cout << "Rx counts : " << recv_count << std::endl;
            commands.push(buff);
        }
    


}
void rc::pcController::close_socket()
{
    close(serverSocket);
}
CMDT rc::pcController::cmdHandler()
{
    CMDT ret = CMDT::RESERVED;
    while(!commands.empty())
    {
        std::string temp = commands.front();
        if (0 == temp.compare(std::string("Close") ))
        {
            /*Close socket*/
            std::cout << "Socket Closed" <<std::endl;
            rc::pcController::close_socket();
            ret = CMDT::CLS_SOCKET;
        } 
        else
        {
            if (0 == temp.compare("Open Gmail"))
            {
                system("xdg-open https://accounts.google.com");
            }
            else if (0 == temp.compare("reboot"))
            {
                system("reboot");
            }
            else if (0 == temp.compare("start vlc"))
            {
                system("cvlc Samplevideo.mp4");
            }
            else if (0 == temp.compare("python3 helloworld.py"))
            {
                system("python3 helloworld.py");
            }
            else
            {
                /*Standard Commands*/
                system(temp.c_str());
            }

            
            ret =  CMDT::USER_CMD;
        }
        /*system delay*/
        sleep(2);
        commands.pop();
    }
    return ret;
}
   