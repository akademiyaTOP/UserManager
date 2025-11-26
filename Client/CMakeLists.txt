add_executable(Client
	main.cpp
	client.h client.cpp
)

target_link_libraries(Client
	Qt6::Core
	Qt6::Widgets
        Qt6::Network
)
