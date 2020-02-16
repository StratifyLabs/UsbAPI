#include <sapi/var.hpp>
#include "UsbLinkTransportDriver.hpp"
#include "usb_link_transport_driver.h"

static const link_transport_mdriver_t usb_link_transport_default_driver = {
	.getname = usb_link_transport_getname,
	.lock = usb_link_transport_lock,
	.unlock = usb_link_transport_unlock,
	.status = usb_link_transport_status,
	.options = 0,
	.phy_driver.handle = LINK_PHY_OPEN_ERROR,
	.phy_driver.open = usb_link_transport_driver_open,
	.phy_driver.write = usb_link_transport_driver_write,
	.phy_driver.read = usb_link_transport_driver_read,
	.phy_driver.close = usb_link_transport_driver_close,
	.phy_driver.flush = usb_link_transport_driver_flush,
	.phy_driver.wait = usb_link_transport_driver_wait,
	.phy_driver.timeout = 100,
	.phy_driver.o_flags = 0,
	.transport_version = 0
};

typedef struct {
	UsbLinkTransportDriver * driver;
} usb_link_transport_handle_t;


void usb_link_transport_load_driver(
		link_transport_mdriver_t * driver
		){
	*driver = usb_link_transport_default_driver;
}

int usb_link_transport_getname(char * dest, const char * last, int len){

	String last_entry;
	String dest_entry;

	if( last != nullptr ){
		last_entry = String(last);
	}

	//return the format vid/pid/serial
	usb::Session session;
	usb::DeviceList device_list = session.get_device_list();

	//where is the last entry
	bool is_next_new = false;
	for(const auto & device: device_list){
		//do any of the descriptors contain StratifyOS
		if( UsbLinkTransportDriver::is_device_stratify_os(device) ){
			String device_path = UsbLinkTransportDriver::build_usb_path(device);
			if( is_next_new || last_entry.is_empty() ){
				dest_entry = device_path;
				break;
			}

			if( device_path == last_entry ){
				is_next_new = true;
			}
		}
	}

	if( dest_entry.is_empty() ){
		return -1;
	}


	strncpy(dest, dest_entry.cstring(), len);
	return 0;
}

int usb_link_transport_lock(link_transport_phy_t handle){

	return 0;
}

int usb_link_transport_unlock(link_transport_phy_t handle){
	return 0;
}

int usb_link_transport_status(link_transport_phy_t handle){
	//check if the USB connection is still valid
	UsbLinkTransportDriver * h = (UsbLinkTransportDriver *)handle;
	if( handle == nullptr ){
		return -1;
	}

	if( h->get_status() < 0 ){
		return LINK_PHY_ERROR;
	}

	return 0;
}

link_transport_phy_t usb_link_transport_driver_open(
		const char * path,
		const void * options
		){
	MCU_UNUSED_ARGUMENT(options);
	UsbLinkTransportDriver * handle = new UsbLinkTransportDriver();
	UsbLinkTransportDriverOptions usb_options(path);

	if( handle->initialize(
				usb_options
				) < 0 ){
		delete handle;
		return LINK_PHY_OPEN_ERROR;
	}

	return handle;
}

int usb_link_transport_driver_write(
		link_transport_phy_t handle,
		const void* buffer,
		int size
		){
	UsbLinkTransportDriver * h = (UsbLinkTransportDriver *)handle;
	if( handle == nullptr ){
		return -1;
	}

	return h->device_handle().write(buffer, usb::DeviceHandle::Size(size));
}

int usb_link_transport_driver_read(
		link_transport_phy_t handle,
		void * buffer,
		int size
		){
	UsbLinkTransportDriver * h = (UsbLinkTransportDriver *)handle;

	if( handle == nullptr ){
		return -1;
	}

	return h->device_handle().read(buffer, usb::DeviceHandle::Size(size));
}

int usb_link_transport_driver_close(
		link_transport_phy_t * handle
		){
	UsbLinkTransportDriver * h = (UsbLinkTransportDriver *)handle;
	if( handle == nullptr ){
		return -1;
	}

	h->finalize();
	delete h;
	return 0;
}

void usb_link_transport_driver_wait(
		int milliseconds){
	chrono::wait(chrono::Milliseconds(milliseconds));
}

void usb_link_transport_driver_flush(
		link_transport_phy_t handle
		){

}

void usb_link_transport_driver_request(
		link_transport_phy_t handle
		){

}

