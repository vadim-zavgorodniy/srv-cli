// local
#include "server_app.hpp"

// local libs
#include <logger/logger.hpp>

// boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

// posix
#include <fstream>

//==============================================================================
using boost::asio::ip::tcp;

//==============================================================================
#define MAX_READ_BUF_SIZE 256
#define CMD_DATA  'D'
#define CMD_END   'E'
#define SEPARATOR ","

//==============================================================================
const char* const msg_socket_connect_error = "Error connecting to \"%1%:%2%\": \"%3%\"";
const char* const msg_socket_io_error      = "Socket input/output error: \"%1%\"";
const char* const msg_proto_err_data       = "Protocol error: not enough data.";
const char* const msg_wrong_fields_count   = "Protocol error: wrong fields count: %1%";
const char* const msg_buffer_overflow      = "Protocol error: data read error. Buffer overflow.";
const char* const msg_unknown_command      = "Unknown command: \"%1%\"";

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
  char buf[MAX_READ_BUF_SIZE];
  char bcmd = '\0';
  uint32_t count = 0;

  bool done = false;
  while (!done)
  {
    // read command
    readData(sock, &bcmd, sizeof(bcmd));

    switch (bcmd)
    {
      case CMD_DATA:
        {
          ++count;

          memset(buf, 0, MAX_READ_BUF_SIZE);
          size_t len = readCmdData(sock, buf, MAX_READ_BUF_SIZE);

          LOG_MESSAGE((boost::format("Data:\n%1%") % buf).str());

          parseData(buf, len);
          break;
        }
      case CMD_END:
        done = true;
        break;
      default:
        throw std::runtime_error(
          (boost::format(msg_unknown_command) % bcmd).str());
    }
  }

  // send a response
  count = htonl(count);
  boost::asio::write(sock, boost::asio::buffer(&count, sizeof(count)));
  LOG_MESSAGE("Done!");
}

//------------------------------------------------------------------------------
void ServerApp::parseData(char* buf, size_t len)
{
  LOG_MESSAGE((boost::format("Parsing: %1%") % buf).str());

  using namespace boost::date_time;
  using namespace boost::local_time;

  std::vector<std::string> items;
  std::string tmp(buf, len);
  boost::algorithm::split(items, tmp, boost::is_any_of(SEPARATOR));

  if (3 != items.size())
    throw std::runtime_error((boost::format(msg_wrong_fields_count) % items.size()).str());

  // std::stringstream ss;
  // local_time_facet* output_facet = new local_time_facet();
  // local_time_input_facet* input_facet = new local_time_input_facet();
  // ss.imbue(std::locale(std::locale::classic(), output_facet));
  // ss.imbue(std::locale(ss.getloc(), input_facet));

}

//------------------------------------------------------------------------------
size_t ServerApp::readCmdData(boost::asio::ip::tcp::socket& sock, char* buf, size_t size)
{
  // read message size
  uint32_t len;
  readData(sock, reinterpret_cast<char*>(&len), sizeof(len));
  len = ntohl(len);

  if (len > MAX_READ_BUF_SIZE)
    throw std::runtime_error(msg_buffer_overflow);

  // read message data
  readData(sock, buf, len);
  return len;
}

//------------------------------------------------------------------------------
size_t ServerApp::readData(boost::asio::ip::tcp::socket& sock, char* buf, size_t size)
{
  boost::system::error_code error;
  size_t rcount = boost::asio::read(sock, boost::asio::buffer(buf, size), error);

  if (rcount != size && error == boost::asio::error::eof)
    throw std::runtime_error(msg_proto_err_data);
  else if (error)
    throw std::runtime_error(
      (boost::format(msg_socket_io_error) % error.message()).str());

  return rcount;
}

//------------------------------------------------------------------------------
