


set(LIBUSB_GIT_REPOSITORY "https://github.com/libusb/libusb.git")
set(LIBUSB_BRANCH "master")

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/lusb/libusb-1.0)
	message(STATUS "Need to clone manually for the first call to cmake")
	execute_process(
		COMMAND git clone --branch ${LIBUSB_BRANCH} ${LIBUSB_GIT_REPOSITORY}
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)

endif()


#[[

project(libusb CXX C)

include(${CMAKE_CURRENT_SOURCE_DIR}/sources.cmake)

set(PRIVATE_SOURCELIST
	${SOURCES})

sos_sdk_library_target(RELEASE libusb "${LIB_OPTION}" release ${SOS_ARCH})

add_library(${RELEASE_TARGET} STATIC)

target_sources(${RELEASE_TARGET}
	PUBLIC
	${PUBLIC_SOURCES}
	PRIVATE
	${PRIVATE_SOURCELIST}
	)

#]]


