cmake_minimum_required (VERSION 3.12)

if(NOT DEFINED API_IS_SDK)
	include(JsonAPI)
endif()

set(LOCAL_NAME UsbAPI)
project(${LOCAL_NAME} CXX)
include(CTest)

option(USBAPI_IS_TEST "Turn ON to enable test build" OFF)

add_subdirectory(lusb lusb)
if(USBAPI_IS_TEST)
	add_subdirectory(tests tests)
endif()
add_subdirectory(library library)

