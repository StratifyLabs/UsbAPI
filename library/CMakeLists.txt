set(DEPENDENCIES JsonAPI SysAPI FsAPI StratifyOS)
api2_add_library(
	NAME UsbAPI
	DEPENDENCIES ${DEPENDENCIES}
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



