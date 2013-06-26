#include "client_app.hpp"
#include "logger.hpp"

#include <boost/asio.hpp>
#include <boost/format.hpp>

#include <fstream>

//==============================================================================
#define MAX_LINE_SIZE 4096

//==============================================================================
const char* const msg_file_open_error = "Ошибка открытия файла \"%1%\"";

//==============================================================================
void ClientApp::run(const std::string& file_name)
{
  std::fstream fin;
//  fin.exceptions(std::fstream::failbit | std::fstream::badbit);
  fin.open(file_name.c_str(), std::fstream::in);
  if (fin.fail() || fin.bad())
    throw std::runtime_error(
      (boost::format(msg_file_open_error) % file_name).str());

  boost::asio::io_service io;
  boost::asio::ip::tcp::socket sock(io);

  std::string line;
  while (!fin.eof()) {
    getline(fin, line);
    LOG_MESSAGE(line);

  }
  // }
  // catch (const std::fstream::failure& e) {
  //   throw std::runtime_error("Ошибка открытия файла \"" +
  //                            file_name + "\" : " + e.what());
  // }
}

//------------------------------------------------------------------------------
void ClientApp::connect()
{
//  m_socket = socket(AF_INET);
}

//------------------------------------------------------------------------------
void ClientApp::disconnect()
{

}

//------------------------------------------------------------------------------
void ClientApp::send(const std::string& data)
{

}

//------------------------------------------------------------------------------
std::string ClientApp::receive()
{
  std::string res;

  return res;
}

//------------------------------------------------------------------------------
