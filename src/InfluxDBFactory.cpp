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

#include "InfluxDB/InfluxDBFactory.h"
#include <functional>
#include <string>
#include <memory>
#include <map>
#include "InfluxDB/InfluxDBException.h"
#include "InfluxDB/UriParser.h"
#include "InfluxDB/HTTP.h"
#include "InfluxDB/BoostSupport.h"
#include "telemetry_m4.h"
namespace influxdb
{
    namespace internal
    {
        std::unique_ptr<HTTP> withHttpTransport(const std::string& url)
        {
            auto urlCopy = url;
            http::url uri = http::ParseHttpUrl(urlCopy);
            auto transport = std::make_unique<HTTP>(uri.url, uri.port);
            if (!uri.user.empty() && !uri.password.empty())
            {
                transport->setBasicAuthentication(uri.user, uri.password);
            }
            else if (!uri.password.empty())
            {
                transport->setAuthToken(uri.password);
            }
            return transport;
        }

    }

    std::unique_ptr<InfluxDB> InfluxDBFactory::Get(const std::string& url)
    {
        return std::make_unique<InfluxDB>(internal::withHttpTransport(url));
    }

} // namespace influxdb
