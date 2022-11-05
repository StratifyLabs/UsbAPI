cmake_minimum_required (VERSION 3.12)
set(LOCAL_NAME UsbAPI)
project(UsbAPI
	LANGUAGES CXX
	VERSION 1.3.0)

option(USBAPI_IS_TEST "Turn ON to enable test build" OFF)

add_subdirectory(lusb lusb)
if(USBAPI_IS_TEST)
	add_subdirectory(tests tests)
endif()
add_subdirectory(library library)

