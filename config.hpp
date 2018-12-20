#pragma once

#include <iostream>
#include <list>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/program_options.hpp>

namespace pt = boost::property_tree;
namespace po = boost::program_options; 

namespace dashbutton {

class config {
    public:
        static constexpr const char* ARG_HELP   = "help";
        static constexpr const char* ARG_PORT   = "port";
        static constexpr const char* ARG_DEV    = "dev";
        static constexpr const char* ARG_CONFIG = "config";
        
        
        typedef struct {
            std::string  ip;
            std::string  topic;
            std::string  message;
        } dashbutton_t;

        typedef struct {
            std::string host;
            uint16_t  port;
        } mqtt_server_t;
        
        typedef struct {
            uint16_t     port;
            std::string  deviceName;
        } server_config_t;

    public:
        config(const std::string &file)
        : fileName(file) {
            try {
                pt::ptree     configTree;
                pt::xml_parser::read_xml(fileName, configTree);

                serverConfig.port = configTree.get<uint16_t>("dashbutton.server.port");
                serverConfig.deviceName = configTree.get<std::string>("dashbutton.server.device");
                
                mqttServer.host = configTree.get<std::string>("dashbutton.mqtt.host");
                mqttServer.port = configTree.get<uint16_t>("dashbutton.mqtt.port");

                for (const auto &entry : configTree.get_child("dashbutton.clients")) {
                    dashbutton_t btn;
                    const pt::ptree &client = entry.second;
                    btn.ip          = client.get<std::string>("ip");
                    btn.topic       = client.get<std::string>("topic");
                    btn.message     = client.get<std::string>("message");
                    dashbuttonList.push_back(btn);
                }
            }
            catch (pt::xml_parser::xml_parser_error& ex) {
                std::cerr << ex.what() << std::endl;
            }
        }
        ~config() = default;

        /**
         * @brief buttonList
         * @return
         */
        const std::list<dashbutton_t>&  buttonList() const {
            return dashbuttonList;
        }

        /**
         * @brief mqtt
         * @return
         */
        const mqtt_server_t& mqtt() const {
            return mqttServer;
        }
        
        /**
         * @brief server
         * @return 
         */
        const server_config_t& server() const {
            return serverConfig;
        }
        
        /**
         * @brief override setting from XML file, with parameters given on command line
         * @param vm  command line arguments
         */
        void overrideConfigByCommandLine(const po::variables_map &vm) {
            if (vm.count(ARG_PORT))
                serverConfig.port = vm[ARG_PORT].as<uint16_t>();
            if (vm.count(ARG_DEV))
                serverConfig.deviceName = vm[ARG_DEV].as<std::string>();
        }

    private:
        std::string              fileName;
        mqtt_server_t            mqttServer;
        server_config_t          serverConfig;
        std::list<dashbutton_t>  dashbuttonList;
};

} // namespace dashbutton
