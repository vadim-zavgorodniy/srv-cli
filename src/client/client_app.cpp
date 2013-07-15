// local
#include "client_app.hpp"

// local lib
#include <logger/logger.hpp>

// boost
#include <boost/asio.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

// posix
#include <fstream>

//==============================================================================
const char* const msg_file_open_error      = "File open error: \"%1%\"";
const char* const msg_file_read_error      = "File read error: \"%1%\"";
const char* const msg_socket_io_error      = "Socket input/output error: \"%1%\"";
const char* const msg_socket_connect_error = "Error connecting to \"%1%:%2%\": \"%3%\"";

//==============================================================================
using boost::asio::ip::tcp;

//==============================================================================
void ClientApp::run(const std::string& file_name) const
{
  std::fstream fin;
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

      boost::trim(line);
      if (line.empty())
        continue;

      uint32_t sz = line.size();

      // send Data command format: D size data
      sz = htonl(sz);
      boost::asio::write(sock, boost::asio::buffer("D", 1));
      boost::asio::write(sock, boost::asio::buffer(&sz, sizeof(sz)));
      boost::asio::write(sock, boost::asio::buffer(line));
    }

    // End transmission command format: E
    boost::asio::write(sock, boost::asio::buffer("E", 1));

    // read records count
    uint32_t rcount;
    boost::asio::read(sock, boost::asio::buffer(&rcount, sizeof(rcount)));
    rcount = ntohl(rcount);

    LOG_MESSAGE((boost::format("Records count: %1%") % rcount).str());
  }
  catch (const boost::system::system_error& error)
  {
    throw std::runtime_error(
      (boost::format(msg_socket_io_error) % error.what()).str());
  }
}

//------------------------------------------------------------------------------
