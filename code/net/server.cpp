#include "EventLoop.h"
#include "Application.h"
using namespace hxk;

int main() {
    std::shared_ptr<Application> app = std::make_shared<Application>();

    app->Start();
    return 0;
}