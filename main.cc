#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "server.hpp"
#include "config.hpp"
#include "mqtt.hpp"

    
namespace po = boost::program_options; 

static const std::string ARG_HELP("help");
static const std::string ARG_PORT("port");
static const std::string ARG_DEV("dev");
static const std::string ARG_CONFIG("config");


/**
 * @brief main
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char* argv[])
{
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            (ARG_HELP.c_str(), "produce help message")
            (ARG_PORT.c_str(), po::value<int>()->default_value(443), "port to listen on")
            (ARG_CONFIG.c_str(), po::value<std::string>()->default_value("dashbutton.xml"), "configuration file name")
            (ARG_DEV.c_str(), po::value<std::string>()->default_value("enp3s0"), "name of the device to resolve mac address");
        
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    
        
        if (vm.count(ARG_HELP)) {
            std::cout << desc << std::endl;
            return 1;
        }
        
        dashbutton::config config(vm[ARG_CONFIG].as<std::string>());
        dashbutton::mqtt   mqtt(config.mqtt());

        io::io_context io_context;
        const dashbutton::server srv(io_context, config, vm[ARG_PORT].as<int>(), vm[ARG_DEV].as<std::string>());
        io_context.run();
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}
