set(SRCS
	callbacks.c
	compress.c
	host.c
	list.c
	packet.c
	peer.c
	protocol.c
	unix.c
	win32.c
)

engine_add_library(
	LIB libenet
	SRCS ${SRCS}
)

if (NOT LIBENET_FOUND)
	target_include_directories(libenet PUBLIC include/)
	include(CheckFunctionExists)
	include(CheckStructHasMember)
	include(CheckTypeSize)
	check_function_exists("fcntl" HAS_FCNTL)
	check_function_exists("poll" HAS_POLL)
	check_function_exists("getaddrinfo" HAS_GETADDRINFO)
	check_function_exists("getnameinfo" HAS_GETNAMEINFO)
	check_function_exists("gethostbyname_r" HAS_GETHOSTBYNAME_R)
	check_function_exists("gethostbyaddr_r" HAS_GETHOSTBYADDR_R)
	check_function_exists("inet_pton" HAS_INET_PTON)
	check_function_exists("inet_ntop" HAS_INET_NTOP)
	check_struct_has_member("struct msghdr" "msg_flags" "sys/types.h;sys/socket.h" HAS_MSGHDR_FLAGS)
	set(CMAKE_EXTRA_INCLUDE_FILES "sys/types.h" "sys/socket.h")
	check_type_size("socklen_t" HAS_SOCKLEN_T BUILTIN_TYPES_ONLY)
	unset(CMAKE_EXTRA_INCLUDE_FILES)
	#target_compile_definitions(libenet PRIVATE "$<$<CONFIG:Debug>:ENET_DEBUG>")
	if(MSVC)
		add_compile_options(libenet PRIVATE -W3)
	else()
		add_compile_options(libenet PRIVATE -Wno-error)
	endif()

	if(HAS_FCNTL)
		target_compile_definitions(libenet PRIVATE -DHAS_FCNTL=1)
	endif()
	if(HAS_POLL)
		target_compile_definitions(libenet PRIVATE -DHAS_POLL=1)
	endif()
	if(HAS_GETNAMEINFO)
		target_compile_definitions(libenet PRIVATE -DHAS_GETNAMEINFO=1)
	endif()
	if(HAS_GETADDRINFO)
		target_compile_definitions(libenet PRIVATE -DHAS_GETADDRINFO=1)
	endif()
	if(HAS_GETHOSTBYNAME_R)
		target_compile_definitions(libenet PRIVATE -DHAS_GETHOSTBYNAME_R=1)
	endif()
	if(HAS_GETHOSTBYADDR_R)
		target_compile_definitions(libenet PRIVATE -DHAS_GETHOSTBYADDR_R=1)
	endif()
	if(HAS_INET_PTON)
		target_compile_definitions(libenet PRIVATE -DHAS_INET_PTON=1)
	endif()
	if(HAS_INET_NTOP)
		target_compile_definitions(libenet PRIVATE -DHAS_INET_NTOP=1)
	endif()
	if(HAS_MSGHDR_FLAGS)
		target_compile_definitions(libenet PRIVATE -DHAS_MSGHDR_FLAGS=1)
	endif()
	if(HAS_SOCKLEN_T)
		target_compile_definitions(libenet PRIVATE -DHAS_SOCKLEN_T=1)
	endif()

	if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
		set_target_properties(libenet PROPERTIES COMPILE_FLAGS "-Wno-parentheses-equality")
	elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		set_target_properties(libenet PROPERTIES COMPILE_FLAGS "-Wno-parentheses-equality -Wno-address-of-packed-member")
	endif()
	if (WIN32)
		target_compile_definitions(libenet PRIVATE -D_WINSOCK_DEPRECATED_NO_WARNINGS)
		target_link_libraries(libenet INTERFACE "ws2_32;winmm")
	endif()
else()
	if (WIN32)
		target_link_libraries(libenet PUBLIC "ws2_32;winmm")
	endif()
endif()
