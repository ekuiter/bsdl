cmake_minimum_required(VERSION 3.5)
project(bsdl)

if(WIN32)
	set(EXECUTABLE_PATH ${CMAKE_SOURCE_DIR}/bin_win32)
else(WIN32)
	set(EXECUTABLE_PATH ${CMAKE_SOURCE_DIR}/bin_macos)
endif(WIN32)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fno-inline-functions")

function(prepend var prefix)
    set(listVar "")
    foreach(f ${ARGN})
        list(APPEND listVar "${prefix}/${f}")
    endforeach(f)
    set(${var} "${listVar}" PARENT_SCOPE)
endfunction(prepend)

set(SOURCE_FILES main.cpp http/client.cpp http/response.cpp http/request.cpp http/request_chain.cpp
		aggregators/bs/series.cpp aggregators/bs/season.cpp aggregators/bs/episode.cpp
                aggregators/bs/video_file.cpp providers/provider.cpp curses/window.cpp curses/color.cpp
                curses/input.cpp app.cpp curses/stream_modifier.cpp
                curses/stream.cpp curses/terminal.cpp aggregators/bs/bs.cpp http/request_implementation.cpp
                aggregators/bs/episode_file.cpp settings.cpp
                aggregators/season.cpp aggregators/season_view.cpp aggregators/series.cpp aggregators/episode_file.cpp
                aggregators/episode_download.cpp aggregators/episode.cpp)

prepend(SOURCE_FILES src/ ${SOURCE_FILES})
if(WIN32)
	set(SOURCE_FILES ${SOURCE_FILES} res_win32/bsdl.o)
endif(WIN32)
add_executable(bsdl ${SOURCE_FILES})

set(BOOST_ROOT lib/boost)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIR})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
target_link_libraries(bsdl ${CURL_LIBRARY})

include_directories(lib/curlcpp/include)
include_directories(lib/gumbo-parser/src)
include_directories(lib/gumbo-query/src)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686")
	target_link_libraries(bsdl ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib/libboost_system-mgw49-mt-1_61.a)
	target_link_libraries(bsdl ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib/libboost_filesystem-mgw49-mt-1_61.a)

	find_file(CURLDLL libcurl-4.dll)
	file(COPY ${CURLDLL} DESTINATION ${EXECUTABLE_PATH})

	find_library(PDCURSES_LIBRARY pdcurses)
	find_path(PDCURSES_INCLUDE_DIR curses.h)	
	include_directories(${PDCURSES_INCLUDE_DIR})
	target_link_libraries(bsdl ${PDCURSES_LIBRARY})
	find_file(PDCURSESDLL libpdcurses.dll)
	file(COPY ${PDCURSESDLL} DESTINATION ${EXECUTABLE_PATH})

	target_link_libraries(bsdl ${CMAKE_SOURCE_DIR}/lib/curlcpp/libcurlcpp.dll.a)
	file(COPY lib/curlcpp/libcurlcpp.dll DESTINATION ${EXECUTABLE_PATH})

	file(COPY lib/gumbo-parser/libgumbo-1.dll DESTINATION ${EXECUTABLE_PATH})
	
	target_link_libraries(bsdl ${CMAKE_SOURCE_DIR}/lib/gumbo-query/libgq.dll.a)
	file(COPY lib/gumbo-query/libgq.dll DESTINATION ${EXECUTABLE_PATH})
	
	find_file(STDCDLL libstdc++-6.dll)
	file(COPY ${STDCDLL} DESTINATION ${EXECUTABLE_PATH})
	
	find_file(GCCDLL libgcc_s_dw2-1.dll)
	file(COPY ${GCCDLL} DESTINATION ${EXECUTABLE_PATH})
	
else(WIN32) # Mac OS
    target_link_libraries(bsdl ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

	find_package(CURSES REQUIRED)
	include_directories(${CURSES_INCLUDE_DIR})
	target_link_libraries(bsdl ${CURSES_LIBRARIES})

	find_library(CURSES_PANEL_LIBRARY panel)
	target_link_libraries(bsdl ${CURSES_PANEL_LIBRARY})

	set(LIBCURLCPP ${CMAKE_SOURCE_DIR}/lib/curlcpp/libcurlcpp.dylib)
	target_link_libraries(bsdl ${LIBCURLCPP})
	file(COPY ${LIBCURLCPP} DESTINATION ${EXECUTABLE_PATH})

	file(COPY lib/gumbo-parser/libgumbo.dylib DESTINATION ${EXECUTABLE_PATH})

	set(LIBGQ ${CMAKE_SOURCE_DIR}/lib/gumbo-query/libgq.dylib)
	target_link_libraries(bsdl ${LIBGQ})
	file(COPY ${LIBGQ} DESTINATION ${EXECUTABLE_PATH})
    
endif(WIN32)