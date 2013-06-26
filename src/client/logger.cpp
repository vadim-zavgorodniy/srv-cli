#include "logger.hpp"

#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <stdexcept>
#include <sys/file.h>

namespace warmouse {

//==============================================================================
  const char* const msg_log_init_error = "Ошибка открытия файла протокола \"%s\"";
  const char* const msg_set_lock_error = "Ошибка блокировки файла протокола.";

//==============================================================================
// Logger methods
//==============================================================================
  bool Logger::initialize(const std::string& file_name, bool use_lock)
  {
    closeFile();

    m_file = fopen(file_name.c_str(), "a+");
    if (!m_file)
    {
      error(0, errno, msg_log_init_error, file_name.c_str());
      return false;
    }
    m_use_lock = use_lock;
    return true;
  }

//------------------------------------------------------------------------------
  void Logger::log(const std::string& message, LogLevelT level) const
  {
    std::string slevel;
    switch (level)
    {
      case INF: slevel = "INF"; break;
      case WAR: slevel = "WAR"; break;
      case ERR: slevel = "ERR"; break;
      default:  slevel = "MSG";
    }
    FILE *file = (m_file) ? m_file : ::stdout;

    char buf[MAX_MESSAGE_SIZE];
    snprintf(buf, MAX_MESSAGE_SIZE, "%s [%s]: %s",
             program_invocation_short_name, slevel.c_str(), message.c_str());

    FileLock fl(file);
    if (m_use_lock)
      fl.lock();

    fprintf(file, "%s\n", buf);
  }

//------------------------------------------------------------------------------
  void Logger::closeFile()
  {
    if (m_file)
      fclose(m_file);
  }

//==============================================================================
// FileLock methods
//==============================================================================
  FileLock::FileLock(FILE* file):
    m_file(file),
    m_locked(false)
  { }

//------------------------------------------------------------------------------
  FileLock::~FileLock()
  {
    unlock();
  }

//------------------------------------------------------------------------------
  bool FileLock::lock(bool do_throw, useconds_t timeout)
  {
    if (m_file && !m_locked) {
      int time = timeout, step = 1, res;
      while ( (0 != (res = flock(fileno(m_file), LOCK_EX)))
              && (time > 0))
      {
        usleep(step);
        time -= step;
      }
      if (0 == res)
        m_locked = true;
      else if (do_throw)
        throw std::runtime_error(msg_set_lock_error);
    }
    return m_locked;
  }

//------------------------------------------------------------------------------
  void FileLock::unlock()
  {
    if (m_file && m_locked)
      flock(fileno(m_file), LOCK_UN);
  }

//------------------------------------------------------------------------------

} // namespace warmouse
