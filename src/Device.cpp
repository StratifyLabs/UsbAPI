#include "Device.hpp"
#include <sapi/sys/Printer.hpp>

using namespace usb;


Device::Device(libusb_device * device){
	m_device = device;
	load_strings();
}


DeviceDescriptor Device::get_device_descriptor(){
	struct libusb_device_descriptor descriptor;
	libusb_get_device_descriptor(
				m_device,
				&descriptor
				);
	return DeviceDescriptor(descriptor, m_string_list);
}

ConfigurationDescriptor Device::get_configuration_descriptor(
		int configuration_number
		){
	return ConfigurationDescriptor(
				m_device,
				configuration_number,
				m_string_list
				);
}

ConfigurationDescriptor Device::get_active_configuration_descriptor(){
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
		m_string_list.push_back("invalid");

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
