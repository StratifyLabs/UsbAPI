#include <sapi/var.hpp>
#include <sapi/sys.hpp>
#include <sapi/chrono.hpp>
#include "UsbLinkTransportDriver.hpp"
#include "usb_link_transport_driver.h"

static const link_transport_mdriver_t usb_link_transport_default_driver = {
	.getname = usb_link_transport_getname,
	.lock = usb_link_transport_lock,
	.unlock = usb_link_transport_unlock,
	.status = usb_link_transport_status,
	.phy_driver = {
		.handle = LINK_PHY_OPEN_ERROR,
		.open = usb_link_transport_driver_open,
		.write = usb_link_transport_driver_write,
		.read = usb_link_transport_driver_read,
		.close = usb_link_transport_driver_close,
		.wait = usb_link_transport_driver_wait,
		.flush = usb_link_transport_driver_flush,
		.request = nullptr,
		.transport_read = nullptr,
		.transport_write = nullptr,
		.timeout = 100,
		.o_flags = 0
	},
	.dev_name = {0},
	.notify_name = {0},
	.options = nullptr,
	.transport_version = 0
};

typedef struct {
	UsbLinkTransportDriver * driver;
} usb_link_transport_handle_t;


void usb_link_transport_load_driver(
		link_transport_mdriver_t * driver
		){
	memcpy(
				driver,
				&usb_link_transport_default_driver,
				sizeof(link_transport_mdriver_t)
				);
}

int usb_link_transport_getname(char * dest, const char * last, int len){
	String last_entry;
	String dest_entry;

	if( last != nullptr ){
		last_entry = String(last);
	}

	const usb::SessionOptions session_options =
			usb::SessionOptions()
			.set_vendor_id(0x20a0);

	//return the format vid/pid/serial
	static usb::Session session;
	static bool is_first_call = true;
	static usb::DeviceList device_list = session.get_device_list(session_options);

	if( is_first_call == false ){
		if( (last == nullptr) || (last[0] == 0) ){
			device_list = session.get_device_list(session_options);
		}
	} else {
		is_first_call = false;
	}

	//where is the last entry
	bool is_next_new = false;
	for(const usb::Device & device: device_list){
		//do any of the descriptors contain StratifyOS
		if( UsbLinkTransportDriver::is_device_stratify_os(device) ){

			//check the interfaces
			usb::ConfigurationDescriptor first_configuration =
					device.get_configuration_descriptor(0);

			usb::InterfaceList interface_list = first_configuration.interface_list();
			for(const usb::Interface& iface: interface_list){
				usb::InterfaceDescriptorList alternate_setting_list = iface.alternate_settings_list();
				for(const usb::InterfaceDescriptor& iface_descriptor: alternate_setting_list){
					if( UsbLinkTransportDriver::is_interface_stratify_os(iface_descriptor) == true ){
						String device_path = UsbLinkPath(device, iface_descriptor.interface_number()).build_path();

						//check if this interface has a bulk in and bulk out endpoint
						usb::EndpointDescriptorList	endpoint_list = iface_descriptor.endpoint_list();

						bool is_bulk_input = false;
						bool is_bulk_output = false;

						for(const usb::EndpointDescriptor& ep: endpoint_list){
							if( ep.transfer_type() == usb::EndpointDescriptor::transfer_type_bulk ){
								if( ep.is_direction_in() ){
									is_bulk_input = true;
								} else {
									is_bulk_output = true;
								}
							}
						}


						if( is_bulk_input && is_bulk_output ){
							if( is_next_new || last_entry.is_empty() ){
								dest_entry = device_path;
								strncpy(
											dest,
											dest_entry.cstring(),
											static_cast<size_t>(len));
								return 0;
							}

							if( device_path == last_entry ){
								is_next_new = true;
							}
						}
					}
				}
			}
		}
	}

	return -1;
}

int usb_link_transport_lock(link_transport_phy_t handle){
	MCU_UNUSED_ARGUMENT(handle);
	return 0;
}

int usb_link_transport_unlock(link_transport_phy_t handle){
	MCU_UNUSED_ARGUMENT(handle);
	return 0;
}

int usb_link_transport_status(link_transport_phy_t handle){
	//check if the USB connection is still valid
	UsbLinkTransportDriver * h = reinterpret_cast<UsbLinkTransportDriver *>(handle);
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

	for(const usb::Endpoint& ep: handle->device_handle().endpoint_list()){
		if( ep.transfer_type() == usb::EndpointDescriptor::transfer_type_bulk ){
			handle->set_endpoint_address(ep.address() & 0x7f);
		}
	}

	handle->device_handle().seek(handle->endpoint_address());

	return handle;
}

int usb_link_transport_driver_write(
		link_transport_phy_t handle,
		const void* buffer,
		int size
		){
	UsbLinkTransportDriver * h = reinterpret_cast<UsbLinkTransportDriver *>(handle);
	if( handle == nullptr ){
		return -1;
	}

	return h->device_handle().write(
				buffer,
				usb::DeviceHandle::Size(
					static_cast<u32>(size)
					)
				);
}

int usb_link_transport_driver_read(
		link_transport_phy_t handle,
		void * buffer,
		int size
		){
	UsbLinkTransportDriver * h = reinterpret_cast<UsbLinkTransportDriver *>(handle);

	if( handle == nullptr ){
		return -1;
	}

	//printf("%s():%d read %d bytes\n", __FUNCTION__, __LINE__, size);
	int result = h->device_handle().read(
				buffer,
				usb::DeviceHandle::Size(
					static_cast<u32>(size)
					)
				);

	if( result > 0 ){
		return result;
	}

	if( result == LIBUSB_ERROR_TIMEOUT ){
		return 0;
	}

	return LINK_PHY_ERROR;

}

int usb_link_transport_driver_close(
		link_transport_phy_t * handle
		){


	if( handle == nullptr ){
		return -1;
	}

	UsbLinkTransportDriver * h = reinterpret_cast<UsbLinkTransportDriver *>(*handle);
	*handle = nullptr;
	h->finalize();
	delete h;
	return 0;
}

void usb_link_transport_driver_wait(
		int milliseconds){

	if( milliseconds < 2 ){
		return;
	}

	chrono::wait(chrono::Milliseconds(milliseconds));
}

void usb_link_transport_driver_flush(
		link_transport_phy_t handle
		){
	u8 c;
	UsbLinkTransportDriver * h =
			static_cast<UsbLinkTransportDriver *>(handle);

	chrono::MicroTime timeout = h->device_handle().timeout();

	while( usb_link_transport_driver_read(handle, &c, 1) == 1 ){

	}

	h->device_handle().set_timeout(timeout);
}

void usb_link_transport_driver_request(
		link_transport_phy_t handle
		){
	MCU_UNUSED_ARGUMENT(handle);
}

