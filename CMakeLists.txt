cmake_minimum_required(VERSION 3.5)
project(bsdl)

if(WIN32)
	set(EXECUTABLE_PATH ${CMAKE_SOURCE_DIR}/bin_win32)
else(WIN32)
	set(EXECUTABLE_PATH ${CMAKE_SOURCE_DIR}/bin_macos)
endif(WIN32)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/static)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fno-inline-functions")

set(BOOST_ROOT lib/boost)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system filesystem)
include_directories(${Boost_INCLUDE_DIR})
set(BOOST_LIBRARIES ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
include_directories(lib/curlcpp/include)
include_directories(lib/gumbo-parser/src)
include_directories(lib/gumbo-query/src)
include_directories(lib/json)

add_library(http SHARED src/http/client.cpp src/http/response.cpp
  src/http/request.cpp src/http/request_chain.cpp src/http/request_implementation.cpp)
set_target_properties(http PROPERTIES OUTPUT_NAME "bsdl_http")
target_link_libraries(http ${CURL_LIBRARY})

add_library(curses SHARED src/curses/window.cpp src/curses/color.cpp src/curses/input.cpp
  src/curses/stream_modifier.cpp src/curses/stream.cpp src/curses/terminal.cpp)
set_target_properties(curses PROPERTIES OUTPUT_NAME "bsdl_curses")

add_library(aggregators STATIC src/aggregators/aggregator.cpp
  src/aggregators/season.cpp src/aggregators/season_view.cpp src/aggregators/series.cpp
  src/aggregators/episode_file.cpp src/aggregators/episode_download.cpp src/aggregators/episode.cpp)
set_target_properties(aggregators PROPERTIES OUTPUT_NAME "bsdl_aggregators")
target_link_libraries(aggregators ${BOOST_LIBRARIES} http curses)

add_library(bs STATIC src/aggregators/bs/series.cpp src/aggregators/bs/season.cpp
  src/aggregators/bs/episode.cpp src/aggregators/bs/video_file.cpp src/aggregators/bs/bs.cpp 
  src/aggregators/bs/episode_file.cpp src/aggregators/bs/merge_transform.cpp)
set_target_properties(bs PROPERTIES OUTPUT_NAME "bsdl_bs")
target_link_libraries(bs ${BOOST_LIBRARIES} http curses)

add_library(kx STATIC src/aggregators/kx/kx.cpp src/aggregators/kx/series.cpp
  src/aggregators/kx/season.cpp src/aggregators/kx/episode.cpp src/aggregators/kx/episode_file.cpp
  src/aggregators/kx/video_file.cpp src/aggregators/kx/movie_season.cpp src/aggregators/kx/movie_episode.cpp)
set_target_properties(kx PROPERTIES OUTPUT_NAME "bsdl_kx")
target_link_libraries(kx ${BOOST_LIBRARIES} http curses)

add_library(mk STATIC src/aggregators/mk/mk.cpp
  src/aggregators/mk/series.cpp src/aggregators/mk/episode.cpp src/aggregators/mk/episode_file.cpp
  src/aggregators/mk/movie_episode.cpp src/aggregators/mk/movie_season.cpp
  src/aggregators/mk/season.cpp src/aggregators/mk/video_file.cpp)
set_target_properties(mk PROPERTIES OUTPUT_NAME "bsdl_mk")
target_link_libraries(mk ${BOOST_LIBRARIES} http curses)

add_library(core STATIC src/app.cpp src/settings.cpp src/util/platform.cpp src/providers/provider.cpp)
set_target_properties(core PROPERTIES OUTPUT_NAME "bsdl_core")
target_link_libraries(core http curses)

set(SOURCE_FILES src/main.cpp)
if(WIN32)
  set(SOURCE_FILES ${SOURCE_FILES} res_win32/bsdl.o)
endif(WIN32)
      
add_executable(bsdl ${SOURCE_FILES})
target_link_libraries(bsdl http curses aggregators bs kx mk core)

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686")
	target_link_libraries(bsdl ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib/libboost_system-mgw49-mt-1_61.a)
	target_link_libraries(bsdl ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib/libboost_filesystem-mgw49-mt-1_61.a)

	find_file(CURLDLL libcurl-4.dll)
	file(COPY ${CURLDLL} DESTINATION ${EXECUTABLE_PATH})

	find_library(PDCURSES_LIBRARY pdcurses)
	find_path(PDCURSES_INCLUDE_DIR curses.h)	
	include_directories(${PDCURSES_INCLUDE_DIR})
	target_link_libraries(curses ${PDCURSES_LIBRARY})
	find_file(PDCURSESDLL libpdcurses.dll)
	file(COPY ${PDCURSESDLL} DESTINATION ${EXECUTABLE_PATH})

	target_link_libraries(http ${CMAKE_SOURCE_DIR}/lib/curlcpp/libcurlcpp.dll.a)
	file(COPY lib/curlcpp/libcurlcpp.dll DESTINATION ${EXECUTABLE_PATH})

	file(COPY lib/gumbo-parser/libgumbo-1.dll DESTINATION ${EXECUTABLE_PATH})
	
	target_link_libraries(http ${CMAKE_SOURCE_DIR}/lib/gumbo-query/libgq.dll.a)
	file(COPY lib/gumbo-query/libgq.dll DESTINATION ${EXECUTABLE_PATH})
	
	find_file(STDCDLL libstdc++-6.dll)
	file(COPY ${STDCDLL} DESTINATION ${EXECUTABLE_PATH})
	
	find_file(GCCDLL libgcc_s_dw2-1.dll)
	file(COPY ${GCCDLL} DESTINATION ${EXECUTABLE_PATH})
	
else(WIN32) # Mac OS
	target_link_libraries(bsdl ${BOOST_LIBRARIES})

	find_package(CURSES REQUIRED)
	include_directories(${CURSES_INCLUDE_DIR})
	target_link_libraries(curses ${CURSES_LIBRARIES})

	find_library(CURSES_PANEL_LIBRARY panel)
	target_link_libraries(curses ${CURSES_PANEL_LIBRARY})

	set(LIBCURLCPP ${CMAKE_SOURCE_DIR}/lib/curlcpp/libcurlcpp.dylib)
	target_link_libraries(http ${LIBCURLCPP})
	file(COPY ${LIBCURLCPP} DESTINATION ${EXECUTABLE_PATH})

	file(COPY lib/gumbo-parser/libgumbo.dylib DESTINATION ${EXECUTABLE_PATH})

	set(LIBGQ ${CMAKE_SOURCE_DIR}/lib/gumbo-query/libgq.dylib)
	target_link_libraries(http ${LIBGQ})
	file(COPY ${LIBGQ} DESTINATION ${EXECUTABLE_PATH})
    
endif(WIN32)
