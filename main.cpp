#include "pcController.hpp"
/*Alias*/
namespace rc = pcController;
int main()
{
    
    rc::pcController pc;
    pc.pcTransmit(std::string("Please Enter your Commands"));
    pc.startThreads();     
}



    

