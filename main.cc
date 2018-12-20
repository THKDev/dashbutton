#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "server.hpp"
#include "config.hpp"
#include "mqtt.hpp"

namespace po = boost::program_options; 

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
            (dashbutton::config::ARG_HELP, "produce help message")
            (dashbutton::config::ARG_PORT, po::value<uint16_t>()->default_value(443)->required(), "port to listen on")
            (dashbutton::config::ARG_CONFIG, po::value<std::string>()->default_value("dashbutton.xml"), "configuration file name")
            (dashbutton::config::ARG_DEV, po::value<std::string>()->default_value("enp3s0"), "name of the device to resolve mac address");
        
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);    
        
        if (vm.count(dashbutton::config::ARG_HELP)) {
            std::cout << desc << std::endl;
            return 1;
        }
        
        dashbutton::config config(vm[dashbutton::config::ARG_CONFIG].as<std::string>());
        config.overrideConfigByCommandLine(vm);
        
        dashbutton::mqtt   mqtt(config.mqtt());
        io::io_context io_context;
        dashbutton::server srv(io_context, config, mqtt);
        
        io_context.run();
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}
