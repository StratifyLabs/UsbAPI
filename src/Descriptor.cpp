#include "Descriptor.hpp"

using namespace usb;

DeviceDescriptor::DeviceDescriptor(
		const libusb_device_descriptor & value,
		const DescriptorStringList & string_list
		) : Descriptor(&m_device_descriptor, string_list){
	m_device_descriptor = value;
}

var::JsonObject DeviceDescriptor::to_json() const {
	var::JsonObject result;
	result.insert("bLength", var::JsonInteger(length()));
	result.insert("bDescriptorType", var::JsonInteger(type()));
	result.insert("bcdUSB", var::JsonInteger(bcd_usb()));
	result.insert("bcdDevice", var::JsonInteger(bcd_device()));
	result.insert("bDeviceClass", var::JsonInteger(device_class()));
	result.insert("bDeviceSubClass", var::JsonInteger(device_sub_class()));
	result.insert("bDeviceProtocol", var::JsonInteger(device_protocol()));
	result.insert("bMaxPacketSize0", var::JsonInteger(max_control_packet_size()));
	result.insert("idVendor", var::JsonInteger(vendor_id()));
	result.insert("idProduct", var::JsonInteger(product_id()));
	result.insert("iManufacturer", var::JsonString(manufacturer_string()));
	result.insert("iProduct", var::JsonString(product_string()));
	result.insert("iSerialNumber", var::JsonString(serial_number_string()));
	result.insert("bNumConfigurations", var::JsonInteger(configuration_count()));
	return result;
}

var::JsonObject EndpointDescriptor::to_json() const {
	var::JsonObject result;
	result.insert("bLength", var::JsonInteger(length()));
	result.insert("bDescriptorType", var::JsonInteger(type()));
	result.insert("bEndpointAddress", var::JsonInteger(endpoint_address()));
	result.insert("bmAttributes", var::JsonInteger(attributes()));
	result.insert("wMaxPacketSize", var::JsonInteger(max_packet_size()));
	result.insert("bInterval", var::JsonInteger(interval()));
	result.insert("bRefresh", var::JsonInteger(refresh()));
	result.insert("bSynchAddress", var::JsonInteger(synch_address()));
	return result;
}

var::JsonObject InterfaceDescriptor::to_json() const {
	var::JsonObject result;
	result.insert("bLength", var::JsonInteger(length()));
	result.insert("bDescriptorType", var::JsonInteger(type()));
	result.insert("bInterfaceNumber", var::JsonInteger(interface_number()));
	result.insert("bAlternateSetting", var::JsonInteger(alternate_setting()));
	result.insert("bInterfaceClass", var::JsonInteger(interface_class()));
	result.insert("bInterfaceSubClass", var::JsonInteger(interface_sub_class()));
	result.insert("bInterfaceProtocol", var::JsonInteger(interface_protocol()));
	result.insert("bNumEndpoints", var::JsonInteger(endpoint_count()));
	result.insert("iInterface", var::JsonString(interface_string()));
	EndpointDescriptorList list = endpoint_list();
	var::JsonArray array;
	for(const auto & entry: list){
		array.append(entry.to_json());
	}
	result.insert("endpointsList", array);
	return result;
}

var::JsonObject Interface::to_json() const {
	var::JsonObject result;
	result.insert("bNumAlternateSettings", var::JsonInteger(alternate_settings_count()));
	InterfaceDescriptorList list = alternate_settings_list();
	var::JsonArray array;
	for(const auto & entry: list){
		array.append(entry.to_json());
	}
	result.insert("alternateSettingsList", array);
	return result;
}

var::JsonObject ConfigurationDescriptor::to_json() const {
	var::JsonObject result;
	result.insert("bLength", var::JsonInteger(length()));
	result.insert("bDescriptorType", var::JsonInteger(type()));
	result.insert("wTotalLength", var::JsonInteger(total_length()));
	result.insert("bNumInterfaces", var::JsonInteger(interface_count()));
	result.insert("bConfigurationValue", var::JsonInteger(configuration_value()));
	result.insert("iConfiguration", var::JsonString(configuration_string()));
	result.insert("bmAttributes", var::JsonInteger(attributes()));
	result.insert("MaxPower", var::JsonInteger(max_power()));
	InterfaceList list = interface_list();
	var::JsonArray array;
	for(const auto & entry: list){
		array.append(entry.to_json());
	}
	result.insert("interfaceList", array);
	return result;
}

