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

  void run() const;
protected:
  struct RecordT
  {
    RecordT():
      num1(0.0d), num2(0.0d)
    {
      memset(&tm, 0, sizeof(struct tm));
    }

    double num1;
    double num2;
    struct tm tm;
  };

  void processClient(boost::asio::ip::tcp::socket& sock) const;
  size_t readCmdData(boost::asio::ip::tcp::socket& sock, char* buf, size_t size) const;
  size_t readData(boost::asio::ip::tcp::socket& sock, char* buf, size_t size) const;
  ServerApp::RecordT parseRecord(char* buf, size_t len) const;
private:
  unsigned m_port;
};

//==============================================================================
