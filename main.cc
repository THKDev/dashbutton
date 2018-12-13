#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "asio_ethernet.hpp"

#ifdef ASIO_STANDALONE
    namespace io = asio;
#else
    namespace io = boost::asio;
#endif
    
namespace po = boost::program_options; 

static const std::string ARG_HELP("help");
static const std::string ARG_PORT("port");
static const std::string ARG_DEV("dev");

/**
 * @brief The server class
 */
class server
{
    public:
        server(io::io_context& io_context, const short port, const std::string &device)
        : acceptor(io_context, io::ip::tcp::endpoint(io::ip::tcp::v4(), port)),
          networkdevice(device) {
            do_accept();
        }

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

int main(int argc, char* argv[])
{
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            (ARG_HELP.c_str(), "produce help message")
            (ARG_PORT.c_str(), po::value<int>()->default_value(443), "port to listen on")
            (ARG_DEV.c_str(), po::value<std::string>()->default_value("enp3s0"), "name of the device to resolve mac address");
        
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    
        
        if (vm.count(ARG_HELP)) {
            std::cout << desc << std::endl;
            return 1;
        }
        
        io::io_context io_context;
        server srv(io_context, vm[ARG_PORT].as<int>(), vm[ARG_DEV].as<std::string>());
        io_context.run();
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}
