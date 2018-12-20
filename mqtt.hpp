#pragma once

#include <mosquittopp.h>
#include "config.hpp"

namespace dashbutton {

/**
 * @brief https://mosquitto.org/api/files/mosquitto-h.html
 */
class mqtt : public mosqpp::mosquittopp {
    public:
        mqtt(const config::mqtt_server_t& server)
        : mosqpp::mosquittopp(MQTT_CLIENT_ID),
          connected(false) {
            mosqpp::lib_init();
            const auto rc = connect_async(server.host.c_str(), server.port);
            if (rc == MOSQ_ERR_SUCCESS)
                loop_start();
            else
                std::cerr << "Connect to mqtt server failed. (" << rc << ")" << std::endl;
        }
        virtual ~mqtt() {
            loop_stop(true);
            disconnect();
            mosqpp::lib_cleanup();
        }

        int publish(const std::string& topic, const std::string& message) {
            if (!connected) {
                return -1;
            }
            return mosqpp::mosquittopp::publish(nullptr, topic.c_str(), message.length(), message.c_str());
        }

        /**
         * @brief on_connect
         * @param rc
         */
        virtual void on_connect(int rc) override {
            if (rc != MOSQ_ERR_SUCCESS) 
                std::cerr << "Connect to mqtt server failed. (" << rc << ")" << std::endl;
            else {
                connected = true;
                //https://mosquitto.org/api/files/mosquitto-h.html#mosquitto_reconnect_delay_set
                reconnect_delay_set(2, 60, true);
            }
        }

        virtual void on_disconnect(int rc) override {
            connected = false;
            std::cout << "Disconnect from MQTT server. Error code: " << rc << std::endl;
        }

        virtual void on_error() override {
            std::cerr << "MQTT: an error occured." << std::endl;
        }

    private:
        bool connected;
        
        static constexpr const char* MQTT_CLIENT_ID = "dashbutton";
};

} // namespace dashbutton
