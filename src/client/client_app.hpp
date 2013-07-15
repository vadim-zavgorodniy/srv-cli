#pragma once

#include <string>

//==============================================================================
class ClientApp
{
public:
  ClientApp(const std::string& host, unsigned port):
    m_host(host), m_port(port)
  { }

  void run(const std::string& file_name) const;
private:
  std::string m_host;
  unsigned m_port;
};

//==============================================================================
