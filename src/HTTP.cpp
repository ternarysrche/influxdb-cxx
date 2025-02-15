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

void httpc_result_fn_(void* arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{
    (void) (arg);
    (void) (httpc_result);
    (void) (rx_content_len);
    (void) (srv_res);
    (void) (err);
    // httpc_result
    LOG("Received HTTP response: ");
    LOG("%d", httpc_result)
    LOG("result function called!");
}

err_t httpc_headers_done_fn_(httpc_state_t* connection, void* arg, struct pbuf* hdr, u16_t hdr_len, u32_t content_len)
{
    (void) (connection);
    (void) (arg);
    (void) (hdr);
    (void) (hdr_len);
    (void) (content_len);
    return ERR_OK;
}

err_t altcp_recv_fn_(void* arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err)
{
    (void) (arg);
    (void) (tpcb);
    (void) (p);
    (void) (err);
    char* payload = (char*) p->payload;
    char tmp = payload[(p->len) - 1];
    payload[(p->len) - 1] = 0;
    // if (AN ERROR OCCURED ACCORDING TO PAYLOAD)
    LOG("RES: %s", payload);
    payload[(p->len) - 1] = tmp;
    return ERR_OK;
}

namespace influxdb
{
    namespace
    {
        std::string parseIPAddress(const std::string& url)
        {
            const auto startPosition = url.find("//") + 2;
            const auto endPosition = url.find(":", startPosition);
            return url.substr(startPosition, endPosition - startPosition);
        }

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
                LOG("No Database specified");
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
              .auth_token = "HERE"},
          connection_info{
              .proxy_addr = {.addr = 0},
              .proxy_port = 0,
              .use_proxy = false,
              .result_fn = httpc_result_fn_,
              .headers_done_fn = httpc_headers_done_fn_},
          ipaddr{.addr = 0}, port(port_), write_uri("/write?db=" + databaseName), query_base_uri("/query?db=" + databaseName)

    {
        std::string ip_string = parseIPAddress(url.c_str());
        LOG("IP address: %s", ip_string.c_str());
        ipaddr_aton(ip_string.c_str(), &ipaddr);
    }

    std::string HTTP::query(const std::string& query)
    {
        std::string query_uri = query_base_uri + "&q=" + query;
        httpc_get_file(&ipaddr, port, query_uri.c_str(), &connection_info, altcp_recv_fn_, nullptr, &connection_ptr);
        return "";
    }

    void HTTP::setAuthToken(const std::string& token)
    {
        request_info.auth_token = token.c_str();
    }

    void HTTP::send(std::string&& lineprotocol)
    {
        if (request_info.auth_token == nullptr)
        {
            LOG("No authentication token set!");
        }
        std::string uri = "/write?db=" + databaseName;
        request_info.payload = lineprotocol.c_str();
        LOG("Sending HTTP request. lineproc: %s", lineprotocol.c_str());
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
