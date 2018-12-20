# Dash button to MQTT bridge
Simple dash button interceptor. Take IP address of the dash button and send a configured message to a MQTT server. Beware that the dash button has a delay of 2~3 seconds on each press. This because the dash button is connecting every time to the WiFi. Tested on [VIM2 Khadas](https://www.khadas.com/vim) (Arm64) with CentOS 7.4.

## Dash button
A way to keep the WiFi settings. Let the assistant run through the Amazon App and then simply cancel the selection of the product you want to order.

## Setup
### Copy files
* `dashbutton` to `/usr/local/bin/`
* `doc/dashbutton.xml` to `/usr/local/etc/dashbutton/`
* `etc/dashbutton.service` to `/etc/systemd/system/`

### Modify dashbutton.xml
Adjust IP addresses (should be fixed/static addresses), topic and messages according your needs. 

### Redirect dash button request
Redirect host names the dash button is connecting to, to the host the dashbutton interceptor is running.

![OpenWRT settings](https://raw.githubusercontent.com/THKDev/dashbutton/master/doc/wrt-hostnames.png)

Currently following host names are used
* parker-gw-eu.amazon.com
* dash-button-na.amazon.com
* dash-button-eu-aws-opf.amazon.com

Additionaly NTP server of amazon can be blocked. Dash button is asking for current time first before sending a request to amazon.

* 0.amazon.pool.ntp.org
* 1.amazon.pool.ntp.org
* 2.amazon.pool.ntp.org
* 3.amazon.pool.ntp.org

### Start dash button interceptor
* `systemctl start dashbutton`
* `systemctl enable dashbutton` keep dash button running after restarting
* `systemctl enable dashbutton` check that dashbutton is running and listen for messages
  
## Build
Using [ASIO](https://github.com/chriskohlhoff/asio) in STANDALONE mode because CentOS 7 on arm64 providing an older version of boost::asio.

```shell
$ git clone --recursive https://github.com/THKDev/dashbutton.git
$ cd dashbutton
$ mkdir release
$ cd release
$ cmake -DCMAKE_BUILD_TYPE=Release ../ 
$ make
$ bin/dashbutton --help
```
