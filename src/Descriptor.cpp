#include "Descriptor.hpp"

using namespace usb;

DeviceDescriptor::DeviceDescriptor(const libusb_device_descriptor & value
		) : Descriptor(&m_device_descriptor){
	m_device_descriptor = value;
}
