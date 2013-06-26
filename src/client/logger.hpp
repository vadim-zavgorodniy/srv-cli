#pragma once

#include <stdio.h>
#include <string>

namespace warmouse {

//==============================================================================
  const unsigned MAX_MESSAGE_SIZE = 4096;

//==============================================================================
// Simple logger class
//==============================================================================
  class Logger
  {
  public:
    enum LogLevelT {INF, WAR, ERR};

    // singleton
    static Logger& instance()
    {
      static Logger log;
      return log;
    }

    bool initialize(const std::string& file_name, bool use_lock = true);

    void log(const std::string& message, LogLevelT level = INF) const;

  protected:
    void closeFile();

  private:
    Logger(): m_file(NULL), m_use_lock(false) {};
    Logger(const Logger&);
    Logger& operator=(const Logger&);
    ~Logger() { closeFile(); }

    FILE* m_file;
    bool m_use_lock;
  };

//==============================================================================
// Class 4 locking the provided file
//==============================================================================
  class FileLock
  {
  public:
    FileLock(FILE* file);
    ~FileLock();

    bool lock(bool do_throw = false, useconds_t timeout = 2000);
    void unlock();

  private:
    FILE* m_file;
    bool m_locked;
  };

//==============================================================================
#define LOG_MESSAGE(MESSAGE)                                          \
  warmouse::Logger::instance().log((MESSAGE), warmouse::Logger::INF)

//------------------------------------------------------------------------------
#define LOG_WARNING(MESSAGE)                                          \
  warmouse::Logger::instance().log((MESSAGE), warmouse::Logger::WAR)

//------------------------------------------------------------------------------
#define LOG_ERROR(MESSAGE)                                            \
  warmouse::Logger::instance().log((MESSAGE), warmouse::Logger::ERR)
//==============================================================================


} // namespace warmouse
