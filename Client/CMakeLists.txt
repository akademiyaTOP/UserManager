add_executable(Client
	main.cpp
)

target_link_libraries(CLient
	Qt6::Core
	Qt6::Widgets
	Qt6::Network	
)