#pragma once

#include <chrono>
#include <ctime>

#ifdef ASIO_STANDALONE
    #include <asio.hpp>
    namespace io = asio;
#else
    #include <boost/asio.hpp>
    namespace io = boost::asio;
#endif

#include "config.hpp"
#include "mqtt.hpp"

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
    private:
        struct pressed_dashbutton : dashbutton::config::dashbutton_t {
           std::chrono::system_clock::time_point lastpressed;
           
           pressed_dashbutton(const dashbutton::config::dashbutton_t &button) {
               ip = button.ip;
               lastpressed = std::chrono::system_clock::now();
           }
        };
        
    public:
        server(io::io_context& io_context, const config& cfg, const mqtt &mqttC)
        : acceptor(io_context, io::ip::tcp::endpoint(io::ip::tcp::v4(), cfg.server().port)),
          config(cfg),
          mqtt(mqttC) {
            std::cout << "Accept incoming connection on port " << acceptor.local_endpoint().port() << std::endl;
            // ARP not used. IP-addr is used
            // std::cout << "Using device " << config.server().deviceName << " to resolve MAC address." << std::endl;
            do_accept();
        }
        ~server() = default;

    private:
        void do_accept() {
            acceptor.async_accept([this](std::error_code ec, io::ip::tcp::socket socket) {
                try {
                    if (!ec) {
                        const auto &ip = socket.remote_endpoint().address().to_string();
                        const auto &dButtonIter = std::find_if(config.buttonList().cbegin(), config.buttonList().cend(), [ip](dashbutton::config::dashbutton_t btn) {
                            return btn.ip == ip;
                        });
                        if (dButtonIter != config.buttonList().cend()) {
                            auto pButtonIter = std::find_if(pressedButtonList.begin(), pressedButtonList.end(), [dButtonIter](pressed_dashbutton pBtn) {
                               return pBtn.ip == dButtonIter->ip;
                            });
                            
                            if (pButtonIter != pressedButtonList.end())
                                pressedButtonList.erase(pButtonIter);
                            
                            const auto now  = std::chrono::system_clock::now();
                            const auto diff = (pButtonIter == pressedButtonList.end()) ? std::chrono::seconds(5) : std::chrono::duration_cast<std::chrono::seconds>(now - pButtonIter->lastpressed);
                            if (diff > std::chrono::seconds(2)) {
                                pressedButtonList.push_back(pressed_dashbutton(*dButtonIter));
    
                                char *msg;
                                int rc = asprintf(&msg, dButtonIter->message.c_str(), std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
                                if (rc != -1) {
                                    std::cout << "Sending " << msg << " by topic " << dButtonIter->topic << ". Triggered by " << ip << std::endl;
                                    mqtt.publish(dButtonIter->topic, msg);
                                    free(msg);
                                }
                                else {
                                    std::cerr << "asprintf failed. " << std::strerror(errno) << std::endl;
                                }
                            }
                        }
                    }
                }
                catch (std::exception &ex) {
                    std::cerr << "Error while accept. " << ex.what() << std::endl;
                }
                socket.close();
                do_accept();
            });
        }

    private:
        io::ip::tcp::acceptor  acceptor;
        dashbutton::config     config;
        dashbutton::mqtt       mqtt;
        std::list<pressed_dashbutton>  pressedButtonList;
};

} //namespace dashbutton
