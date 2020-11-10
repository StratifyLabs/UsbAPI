
set(DEPENDENCIES JsonAPI FsAPI ChronoAPI VarAPI StratifyOS)


stratifyapi_add_api_library(UsbAPI "${DEPENDENCIES}")



set(LIBUSB ${SOS_SDK_PATH}/Tools/gcc/lib/libusb-1.0.a)

if(SOS_SDK_IS_MACOS)
	target_link_libraries(UsbAPI_release_link
		INTERFACE ${LIBUSB} "-framework Cocoa" "-framework IOKit"
		)

	target_link_libraries(UsbAPI_coverage_link
		INTERFACE ${LIBUSB} "-framework Cocoa" "-framework IOKit"
		)


	target_link_libraries(UsbAPI_debug_link
		INTERFACE ${LIBUSB} "-framework Cocoa" "-framework IOKit"
		)
endif()


