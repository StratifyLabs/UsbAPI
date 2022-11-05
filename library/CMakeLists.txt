
cmsdk2_check_version(
	NAME JsonAPI
	VERSION ${JsonAPI_VERSION}
	MINIMUM_VERSION 1.5.2)

cmsdk2_check_version(
	NAME StratifyOS
	VERSION ${StratifyOS_VERSION}
	MINIMUM_VERSION 4.3.1)

api2_add_library(
	NAME UsbAPI
	DEPENDENCIES JsonAPI SysAPI FsAPI StratifyOS
	TARGETS TARGET_LIST)
if(CMSDK_IS_LINUX)
	message(STATUS "Linking to libusb for linux")
	set(LIBUSB usb-1.0)
else()
	message(STATUS "Using local binaries for windows/mac")
	set(LIBUSB ${CMSDK_LOCAL_PATH}/lib/libusb-1.0.a)
endif()
if(CMSDK_IS_MACOS)
	set(FRAMEWORKS "-framework Cocoa" "-framework IOKit")
elseif(SOS_SDK_IS_LINUX)
	set(FRAMEWORKS "udev")
endif()
foreach(TARGET ${TARGET_LIST})
	target_link_libraries(${TARGET}
		INTERFACE ${LIBUSB} ${FRAMEWORKS})
endforeach()



