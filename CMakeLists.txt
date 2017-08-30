# SETUP

cmake_minimum_required(VERSION 3.5)
project(bsdl)

if(WIN32)
	set(EXECUTABLE_PATH ${CMAKE_SOURCE_DIR}/bin_win32)
	set(LIB_TYPE STATIC)
elseif(APPLE)
	set(EXECUTABLE_PATH ${CMAKE_SOURCE_DIR}/bin_macos)
	set(LIB_TYPE SHARED)
else(WIN32)
	set(EXECUTABLE_PATH ${CMAKE_SOURCE_DIR}/bin_linux)
	set(LIB_TYPE SHARED)
endif(WIN32)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_PATH})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/static)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -fno-inline-functions -fpermissive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined -D__FILE__='\"$(notdir $(abspath $<))\"'")

# EXTERNAL LIBRARIES

set(BOOST_ROOT lib/boost)
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system filesystem unit_test_framework)
include_directories(${Boost_INCLUDE_DIR})
if(WIN32)
set(BOOST_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib/libboost_system-mgw49-mt-1_61.a ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib/libboost_filesystem-mgw49-mt-1_61.a)
elseif(APPLE)
set(BOOST_LIBRARIES ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
set(TEST_LIBRARY ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
else(WIN32)
set(BOOST_LIBRARIES ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib/libboost_system_linux.a ${CMAKE_SOURCE_DIR}/lib/boost/stage/lib/libboost_filesystem_linux.a)
endif(WIN32)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})
include_directories(lib/curlcpp/include)
include_directories(lib/gumbo-parser/src)
include_directories(lib/gumbo-query/src)
include_directories(lib/json)

# TARGETS / DEPENDENCIES

add_library(http ${LIB_TYPE} src/http/client.cpp src/http/response.cpp
  src/http/request.cpp src/http/request_chain.cpp src/http/request_implementation.cpp)
set_target_properties(http PROPERTIES OUTPUT_NAME "bsdl_http")
target_link_libraries(http ${CURL_LIBRARY})

add_library(curses ${LIB_TYPE} src/curses/window.cpp src/curses/color.cpp src/curses/input.cpp
  src/curses/stream_modifier.cpp src/curses/stream.cpp src/curses/terminal.cpp)
set_target_properties(curses PROPERTIES OUTPUT_NAME "bsdl_curses")

add_library(bs STATIC src/aggregators/bs/series.cpp src/aggregators/bs/season.cpp
  src/aggregators/bs/episode.cpp src/aggregators/bs/video_file.cpp src/aggregators/bs/bs.cpp 
  src/aggregators/bs/episode_file.cpp src/aggregators/bs/merge_transform.cpp)
set_target_properties(bs PROPERTIES OUTPUT_NAME "bsdl_bs")
target_link_libraries(bs ${BOOST_LIBRARIES} http curses)

add_library(kx STATIC src/aggregators/kx/kx.cpp src/aggregators/kx/series.cpp
  src/aggregators/kx/season.cpp src/aggregators/kx/episode.cpp src/aggregators/kx/episode_file.cpp
  src/aggregators/kx/video_file.cpp src/aggregators/kx/movie_season.cpp src/aggregators/kx/movie_episode.cpp)
set_target_properties(kx PROPERTIES OUTPUT_NAME "bsdl_kx")
target_link_libraries(kx ${BOOST_LIBRARIES} http curses bs)

add_library(mk STATIC src/aggregators/mk/mk.cpp
  src/aggregators/mk/series.cpp src/aggregators/mk/episode.cpp src/aggregators/mk/episode_file.cpp
  src/aggregators/mk/movie_episode.cpp src/aggregators/mk/movie_season.cpp
  src/aggregators/mk/season.cpp src/aggregators/mk/video_file.cpp)
set_target_properties(mk PROPERTIES OUTPUT_NAME "bsdl_mk")
target_link_libraries(mk ${BOOST_LIBRARIES} http curses bs)

add_library(pr STATIC src/aggregators/pr/series.cpp src/aggregators/pr/season.cpp
  src/aggregators/pr/episode.cpp src/aggregators/pr/video_file.cpp src/aggregators/pr/pr.cpp 
  src/aggregators/pr/episode_file.cpp)
set_target_properties(pr PROPERTIES OUTPUT_NAME "bsdl_pr")
target_link_libraries(pr ${BOOST_LIBRARIES} http curses)

add_library(aggregators STATIC src/aggregators/aggregator.cpp
  src/aggregators/season.cpp src/aggregators/season_view.cpp src/aggregators/series.cpp
  src/aggregators/episode_file.cpp src/aggregators/episode_download.cpp src/aggregators/episode.cpp
  src/aggregators/subtitle.cpp)
set_target_properties(aggregators PROPERTIES OUTPUT_NAME "bsdl_aggregators")
target_link_libraries(aggregators ${BOOST_LIBRARIES} http curses bs kx mk pr)

add_library(core STATIC src/main_app.cpp src/batch_app.cpp src/option.cpp src/settings.cpp
  src/util/platform.cpp src/util/string_similarity.cpp src/providers/provider.cpp)
set_target_properties(core PROPERTIES OUTPUT_NAME "bsdl_core")
target_link_libraries(core http curses aggregators)

set(SOURCE_FILES src/main.cpp)
if(WIN32)
  set(SOURCE_FILES ${SOURCE_FILES} res_win32/bsdl.o)
endif(WIN32)
add_executable(bsdl ${SOURCE_FILES})
target_link_libraries(bsdl ${BOOST_LIBRARIES} http curses aggregators bs kx mk pr core)

add_executable(bsdltest src/test/test_util.cpp src/test/mock_app.cpp src/test/mock_terminal.cpp
  src/test/settings.cpp src/test/provider.cpp src/test/http/request.cpp src/test/http/download_request.cpp
  src/test/http/multi_request.cpp src/test/http/http_fixture.cpp src/test/http/response.cpp
  src/test/http/download_queue.cpp src/test/aggregators/aggregator.cpp src/test/aggregators/download_selection.cpp
  src/test/aggregators/series.cpp src/test/aggregators/season.cpp src/test/aggregators/episode.cpp
  src/test/aggregators/video_file.cpp src/test/aggregators/subtitle.cpp src/test/aggregators/episode_file.cpp
  src/test/aggregators/episode_download.cpp)
target_link_libraries(bsdltest ${BOOST_LIBRARIES} ${TEST_LIBRARY} http curses aggregators bs kx mk pr core)
add_custom_target(check ./bsdltest WORKING_DIRECTORY ${EXECUTABLE_PATH} DEPENDS bsdltest)
add_custom_target(check-quick ./bsdltest --run_test=\!@long_running WORKING_DIRECTORY ${EXECUTABLE_PATH} DEPENDS bsdltest)

# EXTERNAL LIBRARIES

if(WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686")

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
	
elseif(APPLE)
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

else(WIN32)
        set(CURSES_NEED_NCURSES TRUE)
        find_package(Curses REQUIRED)
	include_directories(${CURSES_INCLUDE_DIR})
	target_link_libraries(curses ${CURSES_LIBRARIES})

	find_library(CURSES_PANEL_LIBRARY panel)
	target_link_libraries(curses ${CURSES_PANEL_LIBRARY})

	set(LIBCURLCPP ${CMAKE_SOURCE_DIR}/lib/curlcpp/libcurlcpp.so)
	target_link_libraries(http ${LIBCURLCPP})
	file(COPY ${LIBCURLCPP} DESTINATION ${EXECUTABLE_PATH})

	set(LIBGUMBO ${CMAKE_SOURCE_DIR}/lib/gumbo-parser/libgumbo.so.1)
	target_link_libraries(http ${LIBGUMBO})
	file(COPY ${LIBGUMBO} DESTINATION ${EXECUTABLE_PATH})

        set(LIBGQ ${CMAKE_SOURCE_DIR}/lib/gumbo-query/libgq.so)
	target_link_libraries(http ${LIBGQ})
	file(COPY ${LIBGQ} DESTINATION ${EXECUTABLE_PATH})
    
endif(WIN32)
