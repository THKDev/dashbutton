#pragma once

#ifdef ASIO_STANDALONE
    #include <asio.hpp>
    namespace io = asio;
#else
    #include <boost/asio.hpp>
    namespace io = boost::asio;
#endif

#include <net/if_arp.h>
#include <iostream>

namespace asio {
namespace phy {

class ethernet {
    public:
        typedef io::detail::array<unsigned char, 6> bytes_type;
        
    public:
        ethernet(io::ip::tcp::endpoint epoint, const std::string& devname = "eth0")
        : endpoint(epoint),
          devicename(devname),
          valid(false) {
            if (endpoint.address().is_v4()) {
                valid = macaddr_ipv4();
            }
            else {
                std::cerr << "IPv6 is not supported." << std::endl;
                valid = false;
            }
        }
        
        bytes_type macaddr() const {
            return mac;
        }
        
        std::string to_string() const {
            std::string str(20, '\0');
            if (isValid()) {
                sprintf(&str[0], "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }
            return str;
        }
        
        bool isValid() const {
            return valid;
        }
        
    private:
        bool macaddr_ipv4() {
            struct auto_close {
                auto_close() : fd(-1) { }
               ~auto_close() { if (fd != -1) close(fd); }
               int fd;
            } handle;
            struct arpreq areq;
            struct sockaddr_in *sin;
            struct in_addr addr;
            
            memcpy(&addr.s_addr, endpoint.address().to_v4().to_bytes().data(), 4);
            memset(&areq, 0, sizeof(areq));
            
            sin = (struct sockaddr_in *) &areq.arp_pa;
            sin->sin_family = AF_INET;
            sin->sin_addr = addr;
            
            sin = (struct sockaddr_in *) &areq.arp_ha;
            sin->sin_family = ARPHRD_ETHER;
        
            std::copy_n(devicename.cbegin(), 15, areq.arp_dev);
        
            if ((handle.fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
                perror("socket");
                return false;
            }    
            
            if (ioctl(handle.fd, SIOCGARP, (caddr_t) &areq) == -1) {
                perror("-- Error: unable to make ARP request, error");
                return false;
            }
            
            std::copy_n(areq.arp_ha.sa_data, mac.size(), mac.begin());
            return true;
        }
        
    private:
        io::ip::tcp::endpoint   endpoint;
        bytes_type              mac;
        bool                    valid;
        std::string             devicename;
};

} // namespace phy
} // namespace asio
