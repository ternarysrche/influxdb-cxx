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

#include "UnixSocket.h"
#include "InfluxDB/InfluxDBException.h"
#include <string>

namespace influxdb::transports
{
#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

    UnixSocket::UnixSocket(const std::string& socketPath)
        : mSocket(mIoService), mEndpoint(socketPath)
    {
        mSocket.open();
    }

    void UnixSocket::send(std::string&& message)
    {
        try
        {
            mSocket.send_to(boost::asio::buffer(message, message.size()), mEndpoint);
        }
        catch (const boost::system::system_error& e)
        {
            throw InfluxDBException(e.what());
        }
    }

#else

    UnixSocket::UnixSocket(const std::string&)
    {
        throw InfluxDBException{"Unix socket not supported on this system"};
    }

    void UnixSocket::send(std::string&&)
    {
        throw InfluxDBException{"Unix socket not supported on this system"};
    }

#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

} // namespace influxdb::transports
