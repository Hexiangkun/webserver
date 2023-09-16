#include "Connection.h"
#include "Socket.h"
#include "code/util/Buffer.h"
#include <memory>
#include <iostream>
#include <string>
using namespace hxk;

#define BUFFER_SIZE 1024 
int main() 
{
    std::shared_ptr<Socket> sock = std::make_shared<Socket>();
    sock->Create();
    sock->Connect("127.0.0.1", 8888);
    Connection *conn = new Connection(nullptr, sock->GetFd());

    while (true) {
        std::string input;
        std::getline(std::cin, input);
        conn->SetWriteBuffer(input.c_str());
        conn->HandleWriteEvent();
        if (conn->GetState() == Connection::STATE::Closed) {
        conn->Close();
        break;
        }
        conn->HandleReadEvent();
        std::cout << "Message from server: " << conn->GetReadBuffer()->to_string() << std::endl;
    }

    delete conn;
    return 0;
}
