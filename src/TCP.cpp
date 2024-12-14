// MIT License
//
// Copyright (c) 2021 Felix Moessbauer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

///
/// \author Felix Moessbauer
///

#include "TCP.h"
#include "InfluxDB/InfluxDBException.h"
#include <string>
#include <string.h>
#include "lwip.h"
#include "lwip/tcp.h"
#include "telemetry_m4.h"
#include "lwip/ip_addr.h"
#include "lwip/apps/ping.h"
extern struct netif gnetif;
static bool connected = false;
// static bool link_up = false;

// void tcp_socket_notify_up() {
//     link_up = true;
// }

// void tcp_socket_notify_down() {
//     link_up = false;
// }

namespace influxdb::transports
{
    // namespace ba = boost::asio;

    err_t connected_callback_fun(void* arg, struct tcp_pcb *tpcb, err_t err) {
        (void)(arg);
        (void)(tpcb);
        LOG("finished connecting with error %d", err);
        connected = true;
        return ERR_OK;
    }

    TCP::TCP(const std::string& hostname, int port)
        // : mSocket(mIoService)
    {
        (void)(hostname);
        // NEED TO REWRITE
        iconnected = false;
        this->port = port;
        LOG("constructor called with port %d", port);
        ping_init();
        ping_send_now();
        // pcb = tcp_new();
        // ipaddr_aton(hostname.c_str(), ipaddr);
        // LOG("ipaddr = %lx", ipaddr->addr);
        // tcp_bind(pcb, ipaddr, port); // WHAT'S THE PORT NUMBER
        
        // ba::ip::tcp::resolver resolver(mIoService);
        // ba::ip::tcp::resolver::query query(hostname, std::to_string(port));
        // ba::ip::tcp::resolver::iterator resolverIterator = resolver.resolve(query);
        // ba::ip::tcp::resolver::iterator end;
        // mEndpoint = *resolverIterator;
        // mSocket.open(mEndpoint.protocol());
        // reconnect();
    }

    bool TCP::is_connected() const
    {
        return false;

        // return mSocket.is_open();
    }

    void TCP::reconnect()
    {
        return;

        // mSocket.connect(mEndpoint);
        // mSocket.wait(ba::ip::tcp::socket::wait_write);
    }

    std::string TCP::query(const std::string& query) {
        (void)query;
        std::string notcooked = "you are not cooked";
        return notcooked;
    }

    void TCP::send(std::string&& message)
    {
        // try
        // {
        // message = message;
        // return;
            // message.append("\n");
            if(!iconnected) {
                LOG("iconnected OFF");
                if(netif_is_link_up(&gnetif)) {
                    LOG("now connecting");
                    // tcp_pcb* result = tcp_listen(pcb);
                    // if(result == nullptr) {
                    //     LOG("listen failed!!!");
                    // }
                    // tcp_accept(pcb, connected_callback_fun);
                    tcp_connect(pcb, ipaddr, port, connected_callback_fun);
                    iconnected = true;
                }
                return;
            }
            if(!connected) {
                LOG("not connected yet, aborting");
                return;
            }
            LOG("actually sending");
            if(pcb == nullptr) {
                LOG("pcb is null!!");
            }
            char* message_str = (char*)message.c_str();
            err_t write_result = tcp_write(pcb, (void*)message_str, strlen(message_str), TCP_WRITE_FLAG_COPY);
            err_t output_result = tcp_output(pcb);
            LOG("wrote data with result %d, output with result %d", write_result, output_result);

            // const size_t written = mSocket.write_some(ba::buffer(message, message.size()));
            // if (written != message.size())
            // {
            //     throw InfluxDBException("Error while transmitting data");
            // }
        // }
        // catch // (const boost::system::system_error& e)
        // {
        //     throw InfluxDBException("Error while transmitting data");
        // }
    }

} // namespace influxdb::transports
