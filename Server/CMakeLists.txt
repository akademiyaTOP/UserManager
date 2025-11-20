add_executable(Server
	main.cpp
)

target_link_libraries(Server
	Qt6::Core
	Qt6::Network
	Qt6::Sql
)
