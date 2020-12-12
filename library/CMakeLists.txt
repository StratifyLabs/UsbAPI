
set(DEPENDENCIES JsonAPI FsAPI ChronoAPI VarAPI StratifyOS)

api_add_api_library(UsbAPI "${DEPENDENCIES}")

set(LIBUSB ${SOS_SDK_PATH}/lib/libusb-1.0.a)

if(SOS_SDK_IS_MACOS)
	set(FRAMEWORKS "-framework Cocoa" "-framework IOKit")
elseif(SOS_SDK_IS_LINUX)
	set(FRAMEWORKS "udev")
endif()

target_link_libraries(UsbAPI_release_link
	INTERFACE ${LIBUSB} ${FRAMEWORKS}
	)

target_link_libraries(UsbAPI_coverage_link
	INTERFACE ${LIBUSB} ${FRAMEWORKS}
	)


target_link_libraries(UsbAPI_debug_link
	INTERFACE ${LIBUSB} ${FRAMEWORKS}
	)



