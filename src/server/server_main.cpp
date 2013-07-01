#include <libs/logger/logger.hpp>
#include "server_app.hpp"

#include <stdexcept>

//==============================================================================
const char* const log_file = "../../project.log";
//const char* const usage = "USAGE: server";
const int port = 7777;

//==============================================================================
int main (int argc, char* argv[])
{
  int res = 0;

//  if (!warmouse::Logger::instance().initialize(log_file))
  if (!warmouse::Logger::instance().initialize())
    return 1;

  try {

    // if (2 != argc)
    // {
    //   LOG_ERROR(usage);
    //   return 2;
    // }

    ServerApp app(port);
    app.run();

    LOG_MESSAGE("Привет Warmouse!!!");
  }
  catch (const std::exception& e)
  {
    LOG_ERROR(e.what());
    res = 3;
  }
  catch (...)
  {
    LOG_ERROR("*** Application crash! ***");
    res = 4;
  }

  return res;
}

//------------------------------------------------------------------------------
