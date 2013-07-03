add_executable(client
  client_app.cpp
  client_main.cpp
)

add_dependencies(client logger)

#set_target_properties(client PROPERTIES COMPILE_FLAGS
#  ${generic_compile_options})

target_link_libraries(client
  logger
  boost_system
  pthread
)

install(TARGETS client RUNTIME DESTINATION bin)
