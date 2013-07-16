// local
#include "server_app.hpp"

// local libs
#include <logger/logger.hpp>

// boost
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

// posix
#include <time.h>
#include <math.h>
#include <sys/wait.h>

//==============================================================================
using boost::asio::ip::tcp;

//==============================================================================
#define MAX_READ_BUF_SIZE 256
#define CMD_DATA  'D'
#define CMD_END   'E'
#define SEPARATOR ","

//==============================================================================
const char* const msg_socket_connect_error  = "Error connecting to \"%1%:%2%\": \"%3%\"";
const char* const msg_socket_io_error       = "Socket input/output error: \"%1%\"";
const char* const msg_fork_error            = "Filed to create child process: \"%1%\"";
const char* const msg_set_sig_handler_error = "Filed to set SIGCHLD handler: \"%1%\"";
const char* const msg_unknown_command       = "Protocol error! Unknown command: \"%1%\"";
const char* const msg_proto_err_data        = "Protocol error! Not enough data.";
const char* const msg_wrong_fields_count    = "Protocol error! Wrong fields count: %1%";
const char* const msg_invalid_time_format   = "Protocol error! Invalid date/time format: %1%";
const char* const msg_invalid_num_format    = "Protocol error! Invalid numeric format in record: %1%";
const char* const msg_buffer_overflow       = "Protocol error! Data read error. Buffer overflow.";
const char* const msg_division_by_zero      = "Arithmetic error! Incorrect data.";

//==============================================================================

//------------------------------------------------------------------------------
void on_sig_child(int not_used)
{
  while (0 < waitpid(-1, NULL, WNOHANG)) ;
}

//------------------------------------------------------------------------------
bool setSigHandlers()
{
  struct sigaction act;
  act.sa_handler = on_sig_child;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &act, NULL) < 0)
    return false;
  return true;
}

//------------------------------------------------------------------------------
void ServerApp::run() const
{
  if (!setSigHandlers())
    throw std::runtime_error(
      (boost::format(msg_set_sig_handler_error) % strerror(errno)).str());

  boost::asio::io_service io;
  tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), m_port));

  try
  {
    for (;;)
    {
      tcp::socket sock(io);
      acceptor.accept(sock);

      int pid;

      if ((pid = fork()) < 0)
        throw std::runtime_error(
          (boost::format(msg_fork_error) % strerror(errno)).str());

      else if (pid > 0) /* parent process */
      {
        LOG_DEBUG((boost::format("Spawned process, pid(%1%)") % pid).str());
      }
      else /* child process */
      {
        processClient(sock);
        return;
      }
    }
  }
  catch (const boost::system::system_error& error)
  {
    throw std::runtime_error(
      (boost::format(msg_socket_io_error) % error.what()).str());
  }
}

//------------------------------------------------------------------------------
void ServerApp::processClient(tcp::socket& sock) const
{
  uint32_t count = 0;

  try
  {
    char buf[MAX_READ_BUF_SIZE];
    char bcmd = '\0';
    RecordT max_rec;

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
            const size_t len = readCmdData(sock, buf, MAX_READ_BUF_SIZE);

            const RecordT& rec = parseRecord(buf, len);

            // process record
            struct tm newt = rec.tm;
            struct tm maxt = max_rec.tm;
            if (difftime(mktime(&newt), mktime(&maxt)) > 0)
              max_rec = rec;

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

    if (fabs(max_rec.num2) < 1e-40)
      throw std::runtime_error(msg_division_by_zero);

    char b[80];
    strftime(b, sizeof(b), "%d.%m.%Y %T", &max_rec.tm);
    LOG_MESSAGE((boost::format("%1%: %2%") % b % (max_rec.num1 / max_rec.num2)).str());

    // send a response
    count = htonl(count);
    boost::asio::write(sock, boost::asio::buffer(&count, sizeof(count)));

  }
  catch (const std::runtime_error& e)
  {
    LOG_ERROR(e.what());
  }
}

//------------------------------------------------------------------------------
ServerApp::RecordT ServerApp::parseRecord(char* buf, const size_t len) const
{
// fixed format sample: 03.04.2011 22:14:45,42.323,15.01
  LOG_DEBUG((boost::format("Parsing: %1%") % buf).str());

  std::vector<std::string> items;
  const std::string tmp(buf, len);
  boost::algorithm::split(items, tmp, boost::is_any_of(SEPARATOR));

  if (3 != items.size())
    throw std::runtime_error((boost::format(msg_wrong_fields_count) % items.size()).str());

  RecordT rec;

  if (NULL == strptime(items[0].c_str(), "%d.%m.%Y %T", &rec.tm))
    throw std::runtime_error((boost::format(msg_invalid_time_format) % items[0]).str());

  try {
    rec.num1 = boost::lexical_cast<double>(items[1]);
    rec.num2 = boost::lexical_cast<double>(items[2]);
  }
  catch (const boost::bad_lexical_cast& e) {
    throw std::runtime_error((boost::format(msg_invalid_num_format) % buf).str());
  }

  return rec;
}

//------------------------------------------------------------------------------
size_t ServerApp::readCmdData(tcp::socket& sock, char* buf, size_t size) const
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
size_t ServerApp::readData(tcp::socket& sock, char* buf, size_t size) const
{
  boost::system::error_code error;
  const size_t rcount = boost::asio::read(sock, boost::asio::buffer(buf, size), error);

  if (rcount != size && error == boost::asio::error::eof)
    throw std::runtime_error(msg_proto_err_data);
  else if (error)
    throw std::runtime_error(
      (boost::format(msg_socket_io_error) % error.message()).str());

  return rcount;
}

//------------------------------------------------------------------------------
