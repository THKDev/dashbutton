#pragma once

#include "asio_ethernet.hpp"
#include "config.hpp"

#ifdef ASIO_STANDALONE
    namespace io = asio;
#else
    namespace io = boost::asio;
#endif

namespace dashbutton {

/**
 * @brief The server class
 */
class server
{
    public:
        server(io::io_context& io_context, const config& cfg, const short port, const std::string &device)
        : acceptor(io_context, io::ip::tcp::endpoint(io::ip::tcp::v4(), port)),
          networkdevice(device) {
            do_accept();
        }
        ~server() = default;

    private:
        void do_accept() {
            std::cout << "Accept incoming connection on port " << acceptor.local_endpoint().port() << std::endl;
            std::cout << "Using device " << networkdevice << " to resolve MAC address." << std::endl;
            acceptor.async_accept([this](std::error_code ec, io::ip::tcp::socket socket) {
                if (!ec) {
                    std::cout << "Remote IP : " << socket.remote_endpoint().address().to_string() << std::endl;
                    asio::phy::ethernet eth(socket.remote_endpoint(), networkdevice);
                    if (eth.isValid()) {
                        std::cout << "Remote MAC: " << eth.to_string() << std::endl;
                    }
                    socket.close();
                }
                do_accept();
            });
        }

    private:
        io::ip::tcp::acceptor  acceptor;
        std::string            networkdevice;
};

} //namespace dashbutton
