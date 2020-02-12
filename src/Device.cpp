#include "Device.hpp"

using namespace usb;


Device::Device(libusb_device * device){
	m_device = device;
}


DeviceDescriptor Device::get_device_descriptor(){
	struct libusb_device_descriptor descriptor;
	libusb_get_device_descriptor(
				m_device,
				&descriptor
				);
	return DeviceDescriptor(descriptor);
}

ConfigurationDescriptor Device::get_configuration_descriptor(
		int configuration_number
		){
	return ConfigurationDescriptor(
				m_device,
				configuration_number
				);
}

ConfigurationDescriptor Device::get_active_configuration_descriptor(){
	return ConfigurationDescriptor(
				m_device
				);
}
