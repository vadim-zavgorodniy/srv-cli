add_executable(client
  logger.cpp
  client_app.cpp
  client_main.cpp
)

target_link_libraries(client
  boost_system
  pthread
)

install(TARGETS client RUNTIME DESTINATION bin)
