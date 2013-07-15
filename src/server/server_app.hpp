#pragma once

#include <string>
#include <boost/asio.hpp>

//==============================================================================
struct RecordT
{
  struct tm tm;
  double num1;
  double num2;
};

//==============================================================================
class ServerApp
{
public:
  ServerApp(unsigned port):
    m_port(port)
  { }

  void run();
protected:
  void processClient(boost::asio::ip::tcp::socket& sock) const;
  size_t readCmdData(boost::asio::ip::tcp::socket& sock, char* buf, size_t size) const;
  size_t readData(boost::asio::ip::tcp::socket& sock, char* buf, size_t size) const;
  RecordT parseRecord(const char* buf, size_t len) const;
private:
  unsigned m_port;
};

//==============================================================================
