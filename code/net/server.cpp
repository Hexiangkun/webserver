#include "EventLoop.h"
#include "Application.h"
using namespace hxk;

int main() {
    std::shared_ptr<EventLoop> loop = std::make_shared<EventLoop>();
    std::shared_ptr<Application> app = std::make_shared<Application>(loop);

    loop->Loop();
    return 0;
}