// MIT License
//
// Copyright (c) 2020-2024 offa
// Copyright (c) 2019 Adam Wegrzynek
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
/// \author Adam Wegrzynek <adam.wegrzynek@cern.ch>
///

#include "InfluxDB/HTTP.h"
#include "InfluxDB/InfluxDBException.h"
#include "telemetry_m4.h"

httpc_state_t* connection_ptr = nullptr;

void httpc_result_fn_(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
    (void)(arg);
    (void)(httpc_result);
    (void)(rx_content_len);
    (void)(srv_res);
    (void)(err);
    // httpc_result
    LOG("%d", httpc_result)
    LOG("result function called!");
}

err_t httpc_headers_done_fn_(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len) {
    (void)(connection);
    (void)(arg);
    (void)(hdr);
    (void)(hdr_len);
    (void)(content_len);
    LOG("headers done function called!");
    return ERR_OK;
}

err_t altcp_recv_fn_(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    (void)(arg);
    (void)(tpcb);
    (void)(p);
    (void)(err);
    LOG("recv function called!");
    LOG("HERE'S THE DATA:");
    char* payload = (char*)p->payload;
    LOG("%s", payload);
    return ERR_OK;
}

namespace influxdb
{
    namespace
    {
        std::string parseUrl(const std::string& url)
        {
            const auto questionMarkPosition = url.find('?');

            if (questionMarkPosition == std::string::npos)
            {
                return url;
            }
            if (url.at(questionMarkPosition - 1) == '/')
            {
                return url.substr(0, questionMarkPosition - 1);
            }
            return url.substr(0, questionMarkPosition);
        }

        std::string parseDatabaseName(const std::string& url)
        {
            const auto dbParameterPosition = url.find("?db=");

            if (dbParameterPosition == std::string::npos)
            {
                // throw InfluxDBException{"No Database specified"};
                return "";
            }
            return url.substr(dbParameterPosition + 4);
        }
    }


    HTTP::HTTP(const std::string& url, uint16_t port_)
        : endpointUrl(parseUrl(url)), databaseName(parseDatabaseName(url)),
        request_info{
            .request_type = HTTPC_POST,
            .application_type = HTTPC_JSON,
            .payload = nullptr,
            .auth_token = "zmdqB6ZNwPkTlPUktdFm47qBsD4fJjvgr7oIkSmylMt0sVqAYawL1CeZoMU0EggY2dofB4RwkieywPq25NOBTw=="
        }, connection_info {
            .proxy_addr = {.addr = 0},
            .proxy_port = 0,
            .use_proxy = false,
            .result_fn = httpc_result_fn_,
            .headers_done_fn = httpc_headers_done_fn_
        }, ipaddr {.addr = 0}, port(port_)

    {
        ipaddr_aton(url.c_str(), &ipaddr);
    }

    std::string HTTP::query(const std::string& query)
    {
        std::string uri = "/query?db=" + databaseName + "&q=" + query;
        httpc_get_file(&ipaddr, port, uri.c_str(), &connection_info, altcp_recv_fn_, nullptr, &connection_ptr);
        return "";
    }

    void HTTP::setBasicAuthentication(const std::string& user, const std::string& pass)
    {
        (void)(user);
        (void)(pass);
        // session.SetAuth(cpr::Authentication{user, pass, cpr::AuthMode::BASIC});
    }

    void HTTP::setAuthToken(const std::string& token)
    {
        (void)(token);
        // session.UpdateHeader(cpr::Header{{"Authorization", "Token " + token}});
    }

    void HTTP::send(std::string&& lineprotocol)
    {
        std::string uri = "/write?db=" + databaseName;
        request_info.payload = ("{\"data\":\"" + lineprotocol +'"' + "}").c_str();

        httpc_post_file(&ipaddr, port, uri.c_str(), &request_info, &connection_info,
                    altcp_recv_fn_, nullptr, &connection_ptr);
    }


    std::string HTTP::execute(const std::string& cmd)
    {
        return query(cmd);
    }

    void HTTP::createDatabase()
    {
        std::string uri = "/query?q=CREATE DATABASE " + databaseName;
        httpc_post_file(&ipaddr, port, uri.c_str(), &request_info, &connection_info,
                    altcp_recv_fn_, nullptr, &connection_ptr);
    }

} // namespace influxdb
