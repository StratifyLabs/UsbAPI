#ifndef USBAPI_DESCRIPTOR_HPP
#define USBAPI_DESCRIPTOR_HPP

#include <type_traits>
#include <mcu/types.h>
#include <libusb-1.0/libusb.h>
#include <sapi/var/Vector.hpp>

namespace usb {

template<typename T> class Descriptor {
public:

	Descriptor(){
		m_value = nullptr;
	}

	Descriptor(const T * value){
		m_value = value;
	}

	u8 length() const {
		return m_value->bLength;
	}

	u8 type() const {
		return m_value->bDescriptorType;
	}

protected:
	const T * m_value;
};


class DeviceDescriptor :
		public Descriptor<struct libusb_device_descriptor> {
public:

	DeviceDescriptor(
			const struct libusb_device_descriptor & value
			);

	u8 device_class() const {
		return m_value->bDeviceClass;
	}

	u8 device_sub_class() const {
		return m_value->bDeviceSubClass;
	}

	u8 device_protocol() const {
		return m_value->bDeviceProtocol;
	}

	u8 max_control_packet_size() const {
		return m_value->bMaxPacketSize0;
	}

	u16 vendor_id() const {
		return m_value->idVendor;
	}

	u16 product_id() const {
		return m_value->idProduct;
	}

	u16 bcd_device() const {
		return m_value->bcdDevice;
	}

	u8 i_manufacturer() const {
		return m_value->iManufacturer;
	}

	u8 i_serial_number() const {
		return m_value->iSerialNumber;
	}

	u8 i_product() const {
		return m_value-> 	iProduct;
	}

	u8 configuration_count() const {
		return m_value->bNumConfigurations;
	}

private:
	struct libusb_device_descriptor m_device_descriptor;
};

class EndpointDesciptor:
		public Descriptor<struct libusb_endpoint_descriptor> {
public:

	EndpointDesciptor(
			const struct libusb_endpoint_descriptor * value
			) : Descriptor(value){

	}


	u8 endpoint_address() const {
		return m_value->bEndpointAddress;
	}

	u8 attributes() const {
		return m_value->bmAttributes;
	}

	u16 max_packet_size() const {
		return m_value->wMaxPacketSize;
	}

	u8 interval() const {
		return m_value->bInterval;
	}

	u8 refresh() const {
		return m_value->bRefresh;
	}

	u8 synch_address() const {
		return m_value->bSynchAddress;
	}

	u8 extra_length() const {
		return m_value->extra_length;
	}

};

class InterfaceDescriptor:
		public Descriptor<struct libusb_interface_descriptor> {
public:

	InterfaceDescriptor(
			const struct libusb_interface_descriptor * value
			) : Descriptor(value){

	}

	u8 inteface_number() const {
		return m_value->bInterfaceNumber;
	}

	u8 alternate_setting() const {
		return m_value->bAlternateSetting;
	}

	u8 interface_class() const {
		return m_value->bInterfaceClass;
	}

	u8 interface_sub_class() const {
		return m_value->bInterfaceSubClass;
	}

	u8 interface_protocol() const {
		return m_value->bInterfaceProtocol;
	}

	u8 endpoint_count() const {
		return m_value->bNumEndpoints;
	}

	u8 i_interface() const {
		return m_value->iInterface;
	}

	var::Vector<EndpointDesciptor> endpoint_list() const {
		var::Vector<EndpointDesciptor> result;
		for(size_t i = 0; i < endpoint_count(); i++){
			result.push_back(
						EndpointDesciptor(m_value->endpoint + i)
						);
		}
		return result;
	}

	u8 extra_length() const {
		return m_value->extra_length;
	}

};

class Interface {
public:

	Interface(const struct libusb_interface * value){
		m_value = value;
	}

	u8 alternate_settings_count() const {
		return m_value->num_altsetting;
	}

	var::Vector<InterfaceDescriptor> alternate_settings_list() const {
		var::Vector<InterfaceDescriptor> result;
		for(size_t i = 0; i < alternate_settings_count(); i++){
			result.push_back(
						InterfaceDescriptor(m_value->altsetting + i)
						);
		}
		return result;
	}

private:
	const struct libusb_interface * m_value;
};

class ConfigurationDescriptor :
		public Descriptor<struct libusb_config_descriptor> {
public:

	ConfigurationDescriptor(
			struct libusb_device * device
			){
		//get the active configuration
		libusb_config_descriptor * descriptor;
		libusb_get_active_config_descriptor(
					device,
					&descriptor
					);
		m_value = descriptor;
	}

	ConfigurationDescriptor(
			struct libusb_device * device,
			int configuration_index
			){
		//get the active configuration
		libusb_config_descriptor * descriptor;
		libusb_get_config_descriptor(
					device,
					configuration_index,
					&descriptor
					);
		m_value = descriptor;
	}

	~ConfigurationDescriptor(){
		libusb_free_config_descriptor(
					(struct libusb_config_descriptor*)(m_value)
					);
		m_value = nullptr;
	}


	u16 total_length() const {
		return m_value->wTotalLength;
	}

	u8 interface_count() const {
		return m_value->bNumInterfaces;
	}

	u8 configuration_value() const {
		return m_value->	bConfigurationValue;
	}

	u8 i_configuration() const {
		return m_value->	iConfiguration;
	}

	u8 attributes() const {
		return m_value->bmAttributes;
	}

	u8 max_power() const {
		return m_value->MaxPower;
	}

	var::Vector<Interface> interface_list() const {
		var::Vector<Interface> result;
		for(size_t i = 0; i < interface_count(); i++){
			result.push_back(
						Interface(m_value->interface + i)
						);
		}
		return result;
	}

	u8 extra_length() const {
		return m_value->extra_length;
	}

};



}

#endif // USBAPI_DESCRIPTOR_HPP
