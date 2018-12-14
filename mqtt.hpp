#pragma once

#include <mosquittopp.h>
#include "config.hpp"

namespace dashbutton {

constexpr const char* MQTT_CLIENT_ID = "dashbutton";

/**
 * @brief https://mosquitto.org/api/files/mosquitto-h.html
 */
class mqtt : public mosqpp::mosquittopp {
    public:
        mqtt(const config::mqtt_server_t& server)
        : mosqpp::mosquittopp(MQTT_CLIENT_ID),
          connected(false) {
            mosqpp::lib_init();
            connect_async(server.host.c_str(), server.port);
        }
        virtual ~mqtt() {
            disconnect();
            mosqpp::lib_cleanup();
        }

        int publish(const std::string& topic, const std::string& message) {
            if (!connected)
                return -1;

            return mosqpp::mosquittopp::publish(nullptr, topic.c_str(), message.length(), message.c_str());
        }

        /**
         * @brief on_connect
         * @param rc
         */
        virtual void on_connect(int rc) override {
            connected = (rc == 0);
            //https://mosquitto.org/api/files/mosquitto-h.html#mosquitto_reconnect_delay_set
            reconnect_delay_set(2, 60, true);
        }

        virtual void on_disconnect(int rc) override {
            connected = false;
        }

        virtual void on_error() override {
            std::cerr << "MQTT: an error occured." << std::endl;
        }

    private:
        bool connected;
};

} // namespace dashbutton
