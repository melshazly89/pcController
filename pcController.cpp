#include "pcController.hpp"
#include <thread>
#include <mutex>
#include <condition_variable>
/*Alias*/
namespace rc = pcController;

/*Global variables*/
int rc::pcController::send_count = 0;
int rc::pcController::recv_count = 0;
int rc::pcController::handler_count = 0;
std::mutex mtx;
std::condition_variable cv;
bool dataReady = false;
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
    /*Kill Processes listening to port*/
    system("fuser -k 8080/tcp");
    std::cout <<"End Program" << std::endl;
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
    while(true)
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
                /*Lock till inseting new command to queue*/
                std::unique_lock<std::mutex> lock(mtx);
                commands.push(buff);
                std::cout<< buff <<std::endl;
                dataReady = true;
                /*notify handler thread*/
                cv.notify_one();

        }
    }
}
void rc::pcController::startThreads()
{
    std::thread t1(&rc::pcController::pcReceive ,this);
    std::thread t2(&rc::pcController::cmdHandler ,this);
    t1.detach();  
    t2.join();  
}
void rc::pcController::close_socket()
{
    close(serverSocket);
}
void rc::pcController::cmdHandler()
{
    while(true)
    {
        std::string cmd;
        while(!commands.empty())
        {
            /*Locking*/
            std::unique_lock<std::mutex> lock(mtx);
            /*wait receiver thread*/
            cv.wait(lock,[](){return dataReady;});
            cmd = commands.front();
            commands.pop();
        }
        if (0 == cmd.compare("Open Gmail"))
        {
            system("xdg-open https://accounts.google.com");
        }
        else if (0 == cmd.compare("reboot"))
        {
            system("reboot");
        }
        else if (0 == cmd.compare("start vlc"))
        {
            system("cvlc Samplevideo.mp4");
        }
        else if (0 == cmd.compare("python3 helloworld.py"))
        {
            system("python3 helloworld.py");
        }
        else if (0 == cmd.compare("Close"))
        {
            /*Close socket*/
            std::cout << "Socket Closed" <<std::endl;
            rc::pcController::close_socket();
            break;
        }
        else
        {
            /*Standard Commands*/
            system(cmd.c_str());
        }
    }
}

   