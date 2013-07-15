#include <logger/logger.hpp>
#include "server_app.hpp"

#include <stdexcept>

//==============================================================================
const char* const log_file = "../../project.log";
const int port = 7777;

//==============================================================================
int main (int argc, char* argv[])
{
  int res = 0;

#ifdef _DEBUG
  if (!warmouse::Logger::instance().initialize())
#else
  if (!warmouse::Logger::instance().initialize(log_file))
#endif
    return 1;

  try
  {
    ServerApp app(port);
    app.run();
  }
  catch (const std::exception& e)
  {
    LOG_ERROR(e.what());
    res = 2;
  }
  catch (...)
  {
    LOG_ERROR("*** Application crash! ***");
    res = 3;
  }

  return res;
}

//------------------------------------------------------------------------------
