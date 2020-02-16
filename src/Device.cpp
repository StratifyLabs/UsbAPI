#include "Device.hpp"
#include <sapi/sys/Printer.hpp>

using namespace usb;


Device::Device(libusb_device * device){
	m_device = device;
	load_strings();
}


DeviceDescriptor Device::get_device_descriptor() const{
	struct libusb_device_descriptor descriptor;
	libusb_get_device_descriptor(
				m_device,
				&descriptor
				);
	return DeviceDescriptor(descriptor, m_string_list);
}

ConfigurationDescriptor Device::get_configuration_descriptor(
		int configuration_number
		) const{
	return ConfigurationDescriptor(
				m_device,
				configuration_number,
				m_string_list
				);
}

ConfigurationDescriptor Device::get_active_configuration_descriptor() const{
	return ConfigurationDescriptor(
				m_device,
				m_string_list
				);
}

void Device::load_strings(){
	libusb_device_handle * device_handle;
	if( libusb_open(m_device, &device_handle) == LIBUSB_SUCCESS ){
		var::Data buffer(255);
		int idx = 1;
		int result;

		//strings start at 1 -- 0 is invalid
		m_string_list.push_back("(null)");

		do {
			buffer.fill(0);
			result = libusb_get_string_descriptor_ascii(
						device_handle,
						idx++,
						buffer.to_u8(),
						buffer.size()
						);
			if( result > 0 ){

				m_string_list.push_back(
							var::String(
								buffer.to_char(),
								var::String::Length(result)
								)
							);
			}

		} while( (result > 0) && (idx < 256) );

		libusb_close(device_handle);
	}
}


Device * DeviceList::find(
		VendorId vendor_id,
		ProductId product_id,
		const var::String & serial_number
		){

	for(auto & device: *this){
		DeviceDescriptor device_descriptor = device.get_device_descriptor();
		if( (device_descriptor.vendor_id() == vendor_id.argument()) &&
				(device_descriptor.product_id() == product_id.argument())
				){
			if( serial_number.is_empty() ){
				return &device;
			} else if( device_descriptor.serial_number_string() == serial_number ){
				return &device;
			}
		}
	}

	return nullptr;
}

void DeviceHandle::load_endpoint_list(){
	ConfigurationDescriptor configuration =
			m_device->get_active_configuration_descriptor();
	m_endpoint_list.clear();
	for(const auto & interface: configuration.interface_list() ){
		for(const auto & alternate_setting: interface.alternate_settings_list() ){
			for(const auto & endpoint: alternate_setting.endpoint_list()){
				m_endpoint_list.push_back(
							Endpoint(endpoint).set_interface(
								alternate_setting.interface_number()
								)
							);
			}
		}
	}
}

const Endpoint DeviceHandle::find_endpoint(u8 address) const{
	for(const auto & ep: m_endpoint_list){
		if( ep.address() == (address & 0x7f) ){
			return ep;
		}
	}

	return Endpoint();
}

int DeviceHandle::read(
		void * buf,
		Size size
		) const {
	const Endpoint endpoint = find_endpoint(m_location);
	return transfer(endpoint, buf, size.argument(), false);
}

int DeviceHandle::write(
		const void * buf,
		Size size
		) const {
	const Endpoint endpoint = find_endpoint(m_location);
	return transfer(endpoint, (void*)buf, size.argument(), false);
}

int DeviceHandle::transfer(
		const Endpoint & endpoint,
		void * buf,
		int nbyte,
		bool is_read
		) const{

	if( endpoint.is_valid() == false ){
		return -1;
	}
	int result;
	int bytes_transferred = 0;
	const int max_packet_size = endpoint.max_packet_size();
	int page_size;
	u8 * p = static_cast<u8*>(buf);

	do {

		if( nbyte - bytes_transferred > max_packet_size ){
			page_size = max_packet_size;
		} else {
			page_size = nbyte - bytes_transferred;
		}

		result = transfer_packet(
					endpoint,
					p + bytes_transferred,
					page_size,
					is_read
					);
		if( result > 0 ){
			bytes_transferred += result;
		} else {
			return result;
		}

	} while( (bytes_transferred < nbyte) && (result > 0) );

	//send a zero length packet??
	if( page_size == max_packet_size ){
		transfer_packet(endpoint, nullptr, 0, is_read);
	}

	return bytes_transferred;
}


int DeviceHandle::transfer_packet(
		const Endpoint & endpoint,
		void * buf,
		int nbyte,
		bool is_read
		) const{
	int transferred;
	int result = -1;
	u8 address = is_read ? endpoint.read_address() : endpoint.write_address();
	switch(endpoint.transfer_type()){
		case EndpointDescriptor::transfer_type_bulk:
			result = libusb_bulk_transfer(
						m_handle,
						address,
						(unsigned char*)buf,
						nbyte,
						&transferred,
						m_timeout.milliseconds()
						);
		case EndpointDescriptor::transfer_type_interrupt:
			result = libusb_interrupt_transfer(
						m_handle,
						address,
						(unsigned char*)buf,
						nbyte,
						&transferred,
						m_timeout.milliseconds()
						);
		default:
			return -1;
	}
	if( result == 0 ){
		return transferred;
	}

	return result;

}
