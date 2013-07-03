#include "server_app.hpp"
#include <logger/logger.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>

//==============================================================================
#define MAX_READ_BUF_SIZE 1024

//==============================================================================
const char* const msg_socket_connect_error = "Ошибка соединения с \"%1%:%2%\": \"%3%\"";
const char* const msg_socket_io_error      = "Ошибка ввода/вывода на сокете: \"%1%\"";

//==============================================================================
using boost::asio::ip::tcp;

//==============================================================================
void ServerApp::run()
{
  boost::asio::io_service io;
  tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), m_port));

  try
  {
    for (;;)
    {
      boost::asio::ip::tcp::socket sock(io);
      acceptor.accept(sock);

      processClient(sock);
    }
  }
  catch (const boost::system::system_error& error)
  {
    throw std::runtime_error(
      (boost::format(msg_socket_io_error) % error.what()).str());
  }
}

//------------------------------------------------------------------------------
void ServerApp::processClient(boost::asio::ip::tcp::socket& sock)
{
  boost::system::error_code error;
  char buf[MAX_READ_BUF_SIZE] = "\0";
  // size_t len = sock.read_some(boost::asio::buffer(cmd), err);

  size_t len = boost::asio::read(sock, boost::asio::buffer(buf, 1), error);

  if (error == boost::asio::error::eof)
    throw std::runtime_error("Ошибка протокола: не удалось прочитать команду.");
  else if (error)
    throw std::runtime_error(
      (boost::format(msg_socket_io_error) % error.message()).str());

  std::string cmd(buf);
  LOG_MESSAGE(buf);
//  std::cout << strncmp(buf, "D", 1) << std::endl;
  if (cmd != "D")
    throw std::runtime_error(
      (boost::format("Неверная команда: \"%1%\"") % buf[0]).str());

  size_t data_len;
  len = boost::asio::read(sock, boost::asio::buffer(&data_len, sizeof(data_len)), error);

  data_len = be32toh(data_len);
  LOG_MESSAGE((boost::format("Прочитано:\n%1%") % data_len).str());
  data_len = le32toh(data_len);
  LOG_MESSAGE((boost::format("Прочитано:\n%1%") % data_len).str());
}

//------------------------------------------------------------------------------
size_t ServerApp::readSome(boost::asio::ip::tcp::socket& sock, char* buf, size_t size)
{

}

//------------------------------------------------------------------------------
