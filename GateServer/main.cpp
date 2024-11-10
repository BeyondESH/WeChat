#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "const.h"
#include "CServer.h"

int main() {
    try {
        boost::asio::io_context ioc{1};
        unsigned short port = static_cast<unsigned short>(8888);
        boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](boost::system::error_code ec, int signal_number) {
            if (ec) {
                return;
            }
            ioc.stop();
        });
        std::make_shared<CServer>(ioc, port)->start();
        ioc.run();
    } catch (boost::system::system_error &e) {
        std::cerr << "server error:" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return 0;
}
