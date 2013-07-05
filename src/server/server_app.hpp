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
  size_t readCmdData(boost::asio::ip::tcp::socket& sock, char* buf, size_t size);
  size_t readData(boost::asio::ip::tcp::socket& sock, char* buf, size_t size);
  void parseData(char* buf, size_t len);
private:
  unsigned m_port;
};

//==============================================================================
