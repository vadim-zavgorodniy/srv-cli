#pragma once

#include <string>

//==============================================================================
class ClientApp
{
public:
  ClientApp(const std::string& host, unsigned port):
    m_host(host), m_port(port), m_socket(-1)
  { }

  void run(const std::string& file_name);

protected:
  void connect();
  void disconnect();
  void send(const std::string& data);
  std::string receive();

private:
  std::string m_host;
  unsigned m_port;
  int m_socket;
};

//==============================================================================
