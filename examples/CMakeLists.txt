# Add an executable for BasicExample
add_executable(BasicExample BasicExample.cpp)
target_link_libraries(BasicExample SerialCPP)

# Add an executable for ReconnectionExample
add_executable(ReconnectionExample ReconnectionExample.cpp)
target_link_libraries(ReconnectionExample SerialCPP)

# Add an executable for BinaryExample
add_executable(BinaryExample BinaryExample.cpp)
target_link_libraries(BinaryExample SerialCPP)
