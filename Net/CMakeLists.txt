# Sources
file(GLOB SRCS_G "src/*.cpp")
POCO_SOURCES_AUTO(SRCS ${SRCS_G})

# Headers
file(GLOB_RECURSE HDRS_G "include/*.h")
POCO_HEADERS_AUTO(SRCS ${HDRS_G})

# Version Resource
if (MSVC AND BUILD_SHARED_LIBS)
	source_group("Resources" FILES ${PROJECT_SOURCE_DIR}/DLLVersion.rc)
	list(APPEND SRCS ${PROJECT_SOURCE_DIR}/DLLVersion.rc)
endif ()

if(ENABLE_LIBZT)
	set(POCO_SOCKET_IMPL libzt)
	add_compile_definitions(USE_LIBZT=1)
else()
	set(POCO_SOCKET_IMPL bsd)
endif()

add_library(Net ${SRCS})
add_library(Poco::Net ALIAS Net)
set_target_properties(Net
		PROPERTIES
		VERSION ${SHARED_LIBRARY_VERSION} SOVERSION ${SHARED_LIBRARY_VERSION}
		OUTPUT_NAME PocoNet
		DEFINE_SYMBOL Net_EXPORTS
		)

if (${POCO_SOCKET_IMPL} STREQUAL "libzt")
	target_include_directories(Net PUBLIC /opt/homebrew/Cellar/libzt/1.8.1/include)
	target_link_directories(Net PUBLIC /opt/homebrew/Cellar/libzt/1.8.1/lib)
	target_link_libraries(Net PUBLIC Poco::Foundation PUBLIC zt)
else ()
	target_link_libraries(Net PUBLIC Poco::Foundation)
endif ()

# Windows and WindowsCE need additional libraries
if(WIN32)
	target_link_libraries(Net PUBLIC "iphlpapi.lib")
	if(WINCE)
		target_link_libraries(Net PUBLIC "ws2.lib")
	else()
		target_link_libraries(Net PUBLIC "ws2_32.lib")
	endif()
endif(WIN32)

target_include_directories(Net
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

POCO_INSTALL(Net)
POCO_GENERATE_PACKAGE(Net)

if(ENABLE_TESTS)
	add_subdirectory(samples)
	add_subdirectory(testsuite)
endif()
