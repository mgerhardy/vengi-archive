set(LIB libuv)

if(MSVC)
  list(APPEND uv_cflags /W4)
  list(APPEND uv_cflags /wd4701) # potentially uninitialized local variable
  list(APPEND uv_cflags /wd4702) # unreachable code
  list(APPEND uv_cflags /wd4245) # conversion from 'int' to 'uint64_t', signed/unsigned mismatch
  list(APPEND uv_cflags /wd4201) # nonstandard extension used: nameless struct/union
  list(APPEND uv_cflags /wd4456) # declaration of 'q' hides previous local declaration
  list(APPEND uv_cflags /wd4459) # declaration of 'q' hides global declaration
  list(APPEND uv_cflags /wd4206) # nonstandard extension used: translation unit is empty
  list(APPEND uv_cflags /wd4996) # 'wcsrev': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name
  list(APPEND uv_cflags /wd4210) # nonstandard extension used: function given file scope
elseif(CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  foreach(_FLAG
    -fvisibility=hidden --std=gnu89 -Wall -Wextra -Wstrict-prototypes -Wno-unused-parameter -Wno-shadow -Wno-cast-function-type)
    string(REPLACE "=" "_" _NAME ${_FLAG})
    string(REPLACE "-" "_" _NAME ${_NAME})
    check_c_compiler_flag(${_FLAG} HAVE_FLAG${_NAME})
    if (HAVE_FLAG${_NAME})
      list(APPEND uv_cflags ${_FLAG})
    endif()
  endforeach()
endif()

set(uv_sources
    src/fs-poll.c
    src/idna.c
    src/inet.c
    src/random.c
    src/strscpy.c
    src/strtok.c
    src/threadpool.c
    src/timer.c
    src/uv-common.c
    src/uv-data-getter-setters.c
    src/version.c)

if(WIN32)
  list(APPEND uv_defines WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0602)
  list(APPEND uv_libraries
       psapi
       user32
       advapi32
       iphlpapi
       userenv
       ws2_32)
  list(APPEND uv_sources
       src/win/async.c
       src/win/core.c
       src/win/detect-wakeup.c
       src/win/dl.c
       src/win/error.c
       src/win/fs.c
       src/win/fs-event.c
       src/win/getaddrinfo.c
       src/win/getnameinfo.c
       src/win/handle.c
       src/win/loop-watcher.c
       src/win/pipe.c
       src/win/thread.c
       src/win/poll.c
       src/win/process.c
       src/win/process-stdio.c
       src/win/signal.c
       src/win/snprintf.c
       src/win/stream.c
       src/win/tcp.c
       src/win/tty.c
       src/win/udp.c
       src/win/util.c
       src/win/winapi.c
       src/win/winsock.c)
else()
  list(APPEND uv_defines _FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE)
  if(NOT CMAKE_SYSTEM_NAME MATCHES "Android|OS390|QNX")
    # TODO: This should be replaced with find_package(Threads) if possible
    # Android has pthread as part of its c library, not as a separate
    # libpthread.so.
    list(APPEND uv_libraries pthread)
  endif()
  list(APPEND uv_sources
       src/unix/async.c
       src/unix/core.c
       src/unix/dl.c
       src/unix/fs.c
       src/unix/getaddrinfo.c
       src/unix/getnameinfo.c
       src/unix/loop-watcher.c
       src/unix/loop.c
       src/unix/pipe.c
       src/unix/poll.c
       src/unix/process.c
       src/unix/random-devurandom.c
       src/unix/signal.c
       src/unix/stream.c
       src/unix/tcp.c
       src/unix/thread.c
       src/unix/tty.c
       src/unix/udp.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "AIX")
  list(APPEND uv_defines
       _ALL_SOURCE
       _LINUX_SOURCE_COMPAT
       _THREAD_SAFE
       _XOPEN_SOURCE=500
       HAVE_SYS_AHAFS_EVPRODS_H)
  list(APPEND uv_libraries perfstat)
  list(APPEND uv_sources
       src/unix/aix.c
       src/unix/aix-common.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
  list(APPEND uv_defines _GNU_SOURCE)
  list(APPEND uv_libraries dl)
  list(APPEND uv_sources
       src/unix/linux-core.c
       src/unix/linux-inotify.c
       src/unix/linux-syscalls.c
       src/unix/procfs-exepath.c
       src/unix/random-getentropy.c
       src/unix/random-getrandom.c
       src/unix/random-sysctl-linux.c
       src/unix/epoll.c)
endif()

if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Android|Linux")
  list(APPEND uv_sources src/unix/proctitle.c)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "DragonFly|FreeBSD")
  list(APPEND uv_sources src/unix/freebsd.c)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "DragonFly|FreeBSD|NetBSD|OpenBSD")
  list(APPEND uv_sources src/unix/posix-hrtime.c src/unix/bsd-proctitle.c)
endif()

if(APPLE OR CMAKE_SYSTEM_NAME MATCHES "DragonFly|FreeBSD|NetBSD|OpenBSD")
  list(APPEND uv_sources src/unix/bsd-ifaddrs.c src/unix/kqueue.c)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  list(APPEND uv_sources src/unix/random-getrandom.c)
endif()

if(APPLE OR CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
  list(APPEND uv_sources src/unix/random-getentropy.c)
endif()

if(APPLE)
  list(APPEND uv_defines _DARWIN_UNLIMITED_SELECT=1 _DARWIN_USE_64_BIT_INODE=1)
  list(APPEND uv_sources
       src/unix/darwin-proctitle.c
       src/unix/darwin.c
       src/unix/fsevents.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "GNU")
  list(APPEND uv_libraries dl)
  list(APPEND uv_sources
       src/unix/bsd-ifaddrs.c
       src/unix/no-fsevents.c
       src/unix/no-proctitle.c
       src/unix/posix-hrtime.c
       src/unix/posix-poll.c
       src/unix/hurd.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "kFreeBSD")
  list(APPEND uv_defines _GNU_SOURCE)
  list(APPEND uv_libraries dl freebsd-glue)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  list(APPEND uv_defines _GNU_SOURCE _POSIX_C_SOURCE=200112)
  list(APPEND uv_libraries dl rt)
  list(APPEND uv_sources
       src/unix/linux-core.c
       src/unix/linux-inotify.c
       src/unix/linux-syscalls.c
       src/unix/procfs-exepath.c
       src/unix/random-getrandom.c
       src/unix/random-sysctl-linux.c
       src/unix/epoll.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
  list(APPEND uv_sources src/unix/netbsd.c)
  list(APPEND uv_libraries kvm)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
  list(APPEND uv_sources src/unix/openbsd.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "OS390")
  enable_language(CXX)
  list(APPEND uv_defines PATH_MAX=1024)
  list(APPEND uv_defines _AE_BIMODAL)
  list(APPEND uv_defines _ALL_SOURCE)
  list(APPEND uv_defines _ENHANCED_ASCII_EXT=0xFFFFFFFF)
  list(APPEND uv_defines _ISOC99_SOURCE)
  list(APPEND uv_defines _LARGE_TIME_API)
  list(APPEND uv_defines _OPEN_MSGQ_EXT)
  list(APPEND uv_defines _OPEN_SYS_FILE_EXT)
  list(APPEND uv_defines _OPEN_SYS_IF_EXT)
  list(APPEND uv_defines _OPEN_SYS_SOCK_EXT3)
  list(APPEND uv_defines _OPEN_SYS_SOCK_IPV6)
  list(APPEND uv_defines _UNIX03_SOURCE)
  list(APPEND uv_defines _UNIX03_THREADS)
  list(APPEND uv_defines _UNIX03_WITHDRAWN)
  list(APPEND uv_defines _XOPEN_SOURCE=600)
  list(APPEND uv_defines _XOPEN_SOURCE_EXTENDED)
  list(APPEND uv_sources
       src/unix/os390.c
       src/unix/os390-syscalls.c
       src/unix/os390-proctitle.c)
  list(APPEND uv_cflags
       -q64
       -qascii
       -qexportall
       -qgonumber
       -qlongname
       -qlibansi
       -qfloat=IEEE
       -qtune=10
       -qarch=10
       -qasm
       -qasmlib=sys1.maclib:sys1.modgen)
  find_library(ZOSLIB
    NAMES zoslib
    PATHS ${ZOSLIB_DIR}
    PATH_SUFFIXES lib
  )
  list(APPEND uv_libraries ${ZOSLIB})
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "OS400")
  list(APPEND uv_defines
       _ALL_SOURCE
       _LINUX_SOURCE_COMPAT
       _THREAD_SAFE
       _XOPEN_SOURCE=500)
  list(APPEND uv_sources
    src/unix/aix-common.c
    src/unix/ibmi.c
    src/unix/no-fsevents.c
    src/unix/posix-poll.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "SunOS")
  list(APPEND uv_defines __EXTENSIONS__ _XOPEN_SOURCE=500 _REENTRANT)
  list(APPEND uv_libraries kstat nsl sendfile socket)
  list(APPEND uv_sources
       src/unix/no-proctitle.c
       src/unix/sunos.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Haiku")
  list(APPEND uv_defines _BSD_SOURCE)
  list(APPEND uv_libraries bsd network)
  list(APPEND uv_sources
	  src/unix/haiku.c
	  src/unix/bsd-ifaddrs.c
	  src/unix/no-fsevents.c
	  src/unix/no-proctitle.c
	  src/unix/posix-hrtime.c
	  src/unix/posix-poll.c)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
  list(APPEND uv_sources
    src/unix/posix-hrtime.c
    src/unix/posix-poll.c
    src/unix/qnx.c
    src/unix/bsd-ifaddrs.c
    src/unix/no-proctitle.c
    src/unix/no-fsevents.c)
  list(APPEND uv_libraries socket)
endif()

engine_add_library(LIB ${LIB} SRCS ${uv_sources})
if (NOT LIBUV_FOUND)
  target_compile_definitions(${LIB} PRIVATE ${uv_defines} BUILDING_UV_SHARED=1)
  target_compile_options(${LIB} PRIVATE ${uv_cflags})
  target_include_directories(${LIB} PUBLIC include)
  target_include_directories(${LIB} PRIVATE src)
  target_link_libraries(${LIB} PUBLIC ${uv_libraries})
else()
  target_link_libraries(${LIB} INTERFACE ${uv_libraries})
endif()
