#pragma once

#include <iostream>
#include <list>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace pt = boost::property_tree;

namespace dashbutton {

class config {
    public:
        typedef struct {
            std::string mac;
            std::string topic;
            std::string command;
        } dashbutton_t;

        typedef struct {
            std::string host;
            int port;
        } mqtt_server_t;

    public:
        config(const std::string &file)
        : fileName(file) {
            try {
                pt::ptree     configTree;
                pt::xml_parser::read_xml(fileName, configTree);

                mqttServer.host = configTree.get<std::string>("dashbutton.mqtt.host");
                mqttServer.port = configTree.get<int>("dashbutton.mqtt.port");

                for (const auto &entry : configTree.get_child("dashbutton.clients")) {
                    dashbutton_t btn;
                    const pt::ptree &client = entry.second;
                    btn.mac     = client.get<std::string>("mac");
                    btn.topic   = client.get<std::string>("topic");
                    btn.command = client.get<std::string>("command");
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

    private:
        std::string   fileName;
        mqtt_server_t  mqttServer;
        std::list<dashbutton_t>  dashbuttonList;
};

} // namespace dashbutton
