#link_directories(
#  ${domainsocket_LIBRARY_DIRS}
#)

add_executable(client
  logger.cpp
  client_app.cpp
  client_main.cpp
)

set_target_properties(client PROPERTIES COMPILE_FLAGS
  "-fvisibility-inlines-hidden -fvisibility=hidden -DPREFIX=\\\"${prefix}\\\"")

target_link_libraries(client
  boost_system
  pthread
)

install(TARGETS client RUNTIME DESTINATION bin)
