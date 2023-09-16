#include "EventLoop.h"
#include "Application.h"
#include "Connection.h"
#include "Socket.h"
#include "code/util/Buffer.h"
#include <iostream>
using namespace hxk;

int main() {

    std::shared_ptr<Application> app = std::make_shared<Application>();
    app->SetOnConnCallback([](Connection *conn) { 
        std::cout << "New connection fd: " << conn->GetSocket()->GetFd() << std::endl; 
    });
    app->SetOnRecvCallback([](Connection *conn) {
        std::cout << "Message from client " << conn->GetReadBuffer()->to_string() << std::endl;
        conn->HandleSendEvent(conn->GetReadBuffer()->to_string());
    });

    app->Start();

    
    return 0;
}