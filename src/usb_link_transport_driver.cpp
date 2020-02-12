
#include "UsbLinkTransportDriver.hpp"
#include "usb_link_transport_driver.h"


typedef struct {
	UsbLinkTransportDriver * driver;
} usb_link_transport_handle_t;

void usb_link_transport_driver_populate(
		link_transport_driver_t * driver
		){
	*driver = {0};
	driver->open = usb_link_transport_driver_open;
	driver->close = usb_link_transport_driver_close;
	driver->write = usb_link_transport_driver_write;
	driver->read = usb_link_transport_driver_read;
	driver->wait = usb_link_transport_driver_wait;
	driver->flush = usb_link_transport_driver_flush;
	driver->request = usb_link_transport_driver_request;

}

link_transport_phy_t usb_link_transport_driver_open(
		const char * path,
		const void * options){
	UsbLinkTransportDriver * handle = new UsbLinkTransportDriver();
	UsbLinkTransportDriverOptions usb_options(
			static_cast<const usb_link_transport_driver_options_t*>(options)
				);




	return handle;
}

int usb_link_transport_driver_write(
		link_transport_phy_t handle,
		const void* buffer,
		int size
		){
	UsbLinkTransportDriver * h = (UsbLinkTransportDriver *)handle;

	return 0;
}

int usb_link_transport_driver_read(
		link_transport_phy_t handle,
		void * buffer,
		int size
		){
	UsbLinkTransportDriver * h = (UsbLinkTransportDriver *)handle;

	return 0;
}

int usb_link_transport_driver_close(
		link_transport_phy_t * handle
		){
	UsbLinkTransportDriver * h = (UsbLinkTransportDriver *)handle;

	return 0;
}

void usb_link_transport_driver_wait(
		int milliseconds){

}

void usb_link_transport_driver_flush(
		link_transport_phy_t handle
		){

}

void usb_link_transport_driver_request(
		link_transport_phy_t handle
		){

}

