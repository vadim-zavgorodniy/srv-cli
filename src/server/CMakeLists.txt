include_directories("../../libs")

add_executable(server
  server_app.cpp
  server_main.cpp
)

add_dependencies(server logger)

#set_target_properties(server PROPERTIES COMPILE_FLAGS
#  ${generic_compile_options})

target_link_libraries(server
  logger
  boost_system
  pthread
)

install(TARGETS server RUNTIME DESTINATION bin)
