add_executable(Server
	main.cpp
        server.h
        server.cpp
	clienthandler.h
	clienthandler.cpp
        database.h database.cpp
)

target_link_libraries(Server
	Qt6::Core
	Qt6::Network
	Qt6::Sql
)
