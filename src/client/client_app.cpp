#include "client_app.hpp"
#include "logger.hpp"

#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>

//==============================================================================
#define MAX_LINE_SIZE 4096

//==============================================================================
const char* const msg_file_open_error      = "Ошибка открытия файла: \"%1%\"";
const char* const msg_file_read_error      = "Ошибка чтения файла: \"%1%\"";
const char* const msg_socket_io_error      = "Ошибка ввода/вывода на сокете: \"%1%\"";
const char* const msg_socket_connect_error = "Ошибка соединения с \"%1%:%2%\": \"%3%\"";

//==============================================================================
using boost::asio::ip::tcp;

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
  tcp::resolver resolver(io);
  tcp::resolver::query query(m_host, boost::lexical_cast<std::string>(m_port));
  tcp::resolver::iterator iendpoint = resolver.resolve(query);
  boost::asio::ip::tcp::socket sock(io);

  try
  {
    boost::asio::connect(sock, iendpoint);
  }
  catch (const boost::system::system_error& error)
  {
    throw std::runtime_error(
      (boost::format(msg_socket_connect_error) % m_host % m_port % error.what()).str());
  }

  std::string line;

  try
  {
    while (!fin.eof())
    {
      std::getline(fin, line);

      if (fin.bad() || (fin.fail() && !fin.eof()))
        throw std::runtime_error(
          (boost::format(msg_file_read_error) % file_name).str());

      LOG_MESSAGE(line);

      size_t sz = line.size();

      // Send Data command format: D size data
      boost::asio::write(sock, boost::asio::buffer("D"));
      boost::asio::write(sock, boost::asio::buffer(&sz, sizeof(sz)));
      boost::asio::write(sock, boost::asio::buffer(line));
    }

    // End transmission command format: E
    boost::asio::write(sock, boost::asio::buffer("E"));
  }
  catch (const boost::system::system_error& error)
  {
    throw std::runtime_error(
      (boost::format(msg_socket_io_error) % error.what()).str());
  }
}

//------------------------------------------------------------------------------
