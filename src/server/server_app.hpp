#pragma once

#include <string>
#include <boost/asio.hpp>

//==============================================================================
class ServerApp
{
public:
  ServerApp(unsigned port):
    m_port(port)
  { }

  void run();
protected:
  void processClient(boost::asio::ip::tcp::socket& sock);
  size_t readSome(boost::asio::ip::tcp::socket& sock, char* buf, size_t size);
private:
  unsigned m_port;
};

//==============================================================================
