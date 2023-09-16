#include "Application.h"
#include "Connection.h"
#include "Socket.h"
#include <iostream>
using namespace hxk;

int main() {
    std::shared_ptr<Application> app = std::make_shared<Application>();

    app->SetOnConnect([](Connection* conn) {
        std::cout << "New connection fd: " 
        << conn->GetSocket()->GetFd() << std::endl;  

    });

    app->SetOnRecv([](Connection* conn) {
        std::cout << "Message from client " << 
        conn->GetReadBuffer()->to_string() << std::endl;
        conn->HandleSendEvent(conn->GetReadBuffer()->to_string());

    });

    app->Start();
    return 0;
}