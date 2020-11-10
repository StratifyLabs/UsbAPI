
#Add sources to the project
set(SOURCES_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/libusb/libusb)
set(OS_SOURCES_PREFIX ${SOURCES_PREFIX}/os)

set(SOURCES
	${SOURCES_PREFIX}/descriptor.c
	${SOURCES_PREFIX}/io.c
	${SOURCES_PREFIX}/libusb.h
	${SOURCES_PREFIX}/strerror.c
	${SOURCES_PREFIX}/version_nano.h
	${SOURCES_PREFIX}/hotplug.c
	${SOURCES_PREFIX}/libusb-1.0.def
	${SOURCES_PREFIX}/libusbi.h
	${SOURCES_PREFIX}/sync.c
	${SOURCES_PREFIX}/core.c
	${SOURCES_PREFIX}/hotplug.h
	${SOURCES_PREFIX}/libusb-1.0.rc
	${SOURCES_PREFIX}/os
	${SOURCES_PREFIX}/version.h
	${OS_SOURCES_PREFIX}/darwin_usb.c
	${OS_SOURCES_PREFIX}/events_windows.h
	${OS_SOURCES_PREFIX}/haiku_usb_raw.h
	${OS_SOURCES_PREFIX}/netbsd_usb.c
	${OS_SOURCES_PREFIX}/threads_posix.c
	${OS_SOURCES_PREFIX}/windows_common.h
	${OS_SOURCES_PREFIX}/darwin_usb.h
	${OS_SOURCES_PREFIX}/haiku_pollfs.cpp
	${OS_SOURCES_PREFIX}/linux_netlink.c
	${OS_SOURCES_PREFIX}/null_usb.c
	${OS_SOURCES_PREFIX}/threads_posix.h
	${OS_SOURCES_PREFIX}/windows_usbdk.c
	${OS_SOURCES_PREFIX}/events_posix.c
	${OS_SOURCES_PREFIX}/haiku_usb.h
	${OS_SOURCES_PREFIX}/linux_udev.c
	${OS_SOURCES_PREFIX}/openbsd_usb.c
	${OS_SOURCES_PREFIX}/threads_windows.c
	${OS_SOURCES_PREFIX}/windows_usbdk.h
	${OS_SOURCES_PREFIX}/events_posix.h
	${OS_SOURCES_PREFIX}/haiku_usb_backend.cpp
	${OS_SOURCES_PREFIX}/linux_usbfs.c
	${OS_SOURCES_PREFIX}/sunos_usb.c
	${OS_SOURCES_PREFIX}/threads_windows.h
	${OS_SOURCES_PREFIX}/windows_winusb.c
	${OS_SOURCES_PREFIX}/events_windows.c
	${OS_SOURCES_PREFIX}/haiku_usb_raw.cpp
	${OS_SOURCES_PREFIX}/linux_usbfs.h
	${OS_SOURCES_PREFIX}/sunos_usb.h
	${OS_SOURCES_PREFIX}/windows_common.c
	${OS_SOURCES_PREFIX}/windows_winusb.h
)



