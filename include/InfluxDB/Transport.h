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
/// \author Adam Wegrzynek
///

#ifndef INFLUXDATA_TRANSPORTINTERFACE_H
#define INFLUXDATA_TRANSPORTINTERFACE_H

#include "InfluxDB/InfluxDBException.h"
#include "InfluxDB/influxdb_export.h"
#include "InfluxDB/Proxy.h"

namespace influxdb
{

    /// \brief Transport interface
    class INFLUXDB_EXPORT Transport
    {
    public:
        Transport() = default;

        virtual ~Transport() = default;

        /// Sends string blob
        virtual void send(std::string&& message) = 0;

        /// Sends request
        virtual inline std::string query([[maybe_unused]] const std::string& query) {
            return "Query not implemented!";
        }

        /// Executes command
        virtual inline std::string execute([[maybe_unused]] const std::string& cmd) {
            return "Execute not implemented!";
        }

        /// Sends request
        virtual inline void createDatabase() {}

        /// Sets proxy
        virtual inline void setProxy([[maybe_unused]] const Proxy& proxy) {}
    };

} // namespace influxdb

#endif // INFLUXDATA_TRANSPORTINTERFACE_H
