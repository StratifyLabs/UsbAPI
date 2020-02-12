#ifndef USBAPI_DESCRIPTOR_HPP
#define USBAPI_DESCRIPTOR_HPP

#include <type_traits>
#include <mcu/types.h>
#include <libusb-1.0/libusb.h>
#include <sapi/var/Vector.hpp>
#include <sapi/var/Json.hpp>

namespace usb {

using DescriptorStringList = var::Vector<var::String>;

template<typename T> class Descriptor {
public:

	Descriptor(
			const T * value,
			const DescriptorStringList & string_list) :
		m_string_list_reference(string_list){
		m_value = value;
	}

	u8 length() const {
		return m_value->bLength;
	}

	u8 type() const {
		return m_value->bDescriptorType;
	}

	const DescriptorStringList & string_list() const {
		return m_string_list_reference;
	}

	const var::String & string(u8 index) const {
		if( index >= string_list().count() ){
			index = 0;
		}
		return string_list().at(index);
	}

protected:
	const T * m_value = nullptr;
	const DescriptorStringList & m_string_list_reference;
};


class EndpointDescriptor:
		public Descriptor<struct libusb_endpoint_descriptor> {
public:

	enum transfer_type {
		transfer_type_control,
		transfer_type_isochronous,
		transfer_type_bulk,
		transfer_type_interrupt,
		transfer_type_none
	};

	EndpointDescriptor(
			const struct libusb_endpoint_descriptor * value,
			const DescriptorStringList & string_list
			) : Descriptor(value, string_list){

	}

	enum transfer_type transfer_type() const {
		u8 bits = attributes() & 0x03;
		if( bits == 0 ){ return transfer_type_control; }
		if( bits == 1 ){ return transfer_type_isochronous; }
		if( bits == 2 ){ return transfer_type_bulk; }
		if( bits == 3 ){ return transfer_type_interrupt; }
		return transfer_type_none;
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

	var::JsonObject to_json() const;


};

using EndpointDescriptorList = var::Vector<EndpointDescriptor>;


class InterfaceDescriptor:
		public Descriptor<struct libusb_interface_descriptor> {
public:

	InterfaceDescriptor(
			const struct libusb_interface_descriptor * value,
			const DescriptorStringList & string_list
			) : Descriptor(value, string_list){

	}

	u8 interface_number() const {
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

	const var::String & interface_string() const {
		return string( i_interface() );
	}

	EndpointDescriptorList endpoint_list() const {
		EndpointDescriptorList result;
		for(size_t i = 0; i < endpoint_count(); i++){
			result.push_back(
						EndpointDescriptor(m_value->endpoint + i, string_list())
						);
		}
		return result;
	}

	u8 extra_length() const {
		return m_value->extra_length;
	}

	var::JsonObject to_json() const;


};


using InterfaceDescriptorList = var::Vector<InterfaceDescriptor>;


class Interface {
public:

	Interface(
			const struct libusb_interface * value,
			const DescriptorStringList & string_list
			) : m_string_list(string_list){
		m_value = value;
	}

	u8 alternate_settings_count() const {
		return m_value->num_altsetting;
	}

	InterfaceDescriptorList alternate_settings_list() const {
		InterfaceDescriptorList result;
		for(size_t i = 0; i < alternate_settings_count(); i++){
			result.push_back(
						InterfaceDescriptor(m_value->altsetting + i, m_string_list)
						);
		}
		return result;
	}

	var::JsonObject to_json() const;

private:
	const struct libusb_interface * m_value;
	const DescriptorStringList & m_string_list;
};

using InterfaceList = var::Vector<Interface>;

class ConfigurationDescriptor :
		public Descriptor<struct libusb_config_descriptor> {
public:

	ConfigurationDescriptor(
			struct libusb_device * device,
			const DescriptorStringList & string_list
			) : Descriptor(nullptr, string_list){
		//get the active configuration
		m_device = device;
		m_is_active_configuration = true;
		m_configuration_index = 0;
		load_descriptors();
	}

	ConfigurationDescriptor(
			struct libusb_device * device,
			int configuration_index,
			const DescriptorStringList & string_list
			) : Descriptor(nullptr, string_list){
		//get the active configuration
		m_device = device;
		m_is_active_configuration = false;
		m_configuration_index = configuration_index;
		load_descriptors();
	}

	ConfigurationDescriptor(const ConfigurationDescriptor & a) :
		Descriptor(nullptr, a.string_list()){
		copy_configuration(a);
	}

	ConfigurationDescriptor& operator = (const ConfigurationDescriptor & a){
		copy_configuration(a);
		return *this;
	}

	ConfigurationDescriptor(ConfigurationDescriptor && a)
		: Descriptor(nullptr, a.string_list()){
		std::swap(*this, a);
	}

	ConfigurationDescriptor& operator = (ConfigurationDescriptor && a){
		std::swap(*this, a);
		return *this;
	}

	~ConfigurationDescriptor(){
		if( m_value != nullptr ){
			libusb_free_config_descriptor(
						(struct libusb_config_descriptor*)(m_value)
						);
			m_value = nullptr;
		}
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

	const var::String & configuration_string() const {
		return string( i_configuration() );
	}

	u8 attributes() const {
		return m_value->bmAttributes;
	}

	u8 max_power() const {
		return m_value->MaxPower;
	}

	InterfaceList interface_list() const {
		InterfaceList result;
		for(size_t i = 0; i < interface_count(); i++){
			result.push_back(
						Interface(m_value->interface + i, string_list())
						);
		}
		return result;
	}

	u8 extra_length() const {
		return m_value->extra_length;
	}

	var::JsonObject to_json() const;

private:
	struct libusb_device * m_device = nullptr;
	u8 m_configuration_index = 0;
	bool m_is_active_configuration = false;

	void load_descriptors(){
		libusb_config_descriptor * descriptor;
		if( m_is_active_configuration ){
			libusb_get_active_config_descriptor(
						m_device,
						&descriptor
						);
		} else {
			libusb_get_config_descriptor(
						m_device,
						m_is_active_configuration,
						&descriptor
						);
		}
		m_value = descriptor;
	}

	void copy_configuration(const ConfigurationDescriptor & a){
		m_device = a.m_device;
		m_is_active_configuration = a.m_is_active_configuration;
		m_configuration_index = a.m_configuration_index;
		load_descriptors();
	}

};

using ConfigurationDescriptorList = var::Vector<ConfigurationDescriptor>;

class DeviceDescriptor :
		public Descriptor<struct libusb_device_descriptor> {
public:

	DeviceDescriptor(
			const libusb_device_descriptor & value,
			const DescriptorStringList & string_list
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

	u16 bcd_usb() const {
		return m_value->bcdUSB;
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
		return m_value->iProduct;
	}

	const var::String & product_string() const {
		return string( i_product() );
	}

	const var::String & manufacturer_string() const {
		return string( i_manufacturer() );
	}

	const var::String & serial_number_string() const {
		return string( i_serial_number() );
	}

	u8 configuration_count() const {
		return m_value->bNumConfigurations;
	}

	var::JsonObject to_json() const;

private:
	struct libusb_device_descriptor m_device_descriptor;
};

}

#endif // USBAPI_DESCRIPTOR_HPP
