#ifndef USBAPI_DEVICE_HPP
#define USBAPI_DEVICE_HPP

#include <sapi/fs/File.hpp>
#include "Descriptor.hpp"

namespace usb {

class DeviceFlags {
public:
	using VendorId = arg::Argument<u16, struct DeviceFlagsVendorIdTag>;
	using ProductId = arg::Argument<u16, struct DeviceFlagsProductIdTag>;

};

class Endpoint {
public:

	Endpoint(){
		m_transfer_type = EndpointDescriptor::transfer_type_none;
		m_address = 0;
		m_interface = 0;
		m_max_packet_size = 0;
	}

	Endpoint(const EndpointDescriptor & endpoint_descriptor){
		m_transfer_type = endpoint_descriptor.transfer_type();
		m_address = endpoint_descriptor.endpoint_address() & 0x7f;
		m_max_packet_size = endpoint_descriptor.max_packet_size();
		m_interface = 0;
	}

	bool is_valid() const {
		return m_transfer_type != EndpointDescriptor::transfer_type_none;
	}

	Endpoint& set_transfer_type(enum EndpointDescriptor::transfer_type value){
		m_transfer_type = value;
		return *this;
	}

	Endpoint& set_address(u8 value){
		m_address = value & 0x7f;
		return *this;
	}

	Endpoint& set_interface(u8 value){
		m_interface = value & 0x7f;
		return *this;
	}

	enum EndpointDescriptor::transfer_type transfer_type() const {
		return m_transfer_type;
	}

	u8 address() const {
		return m_address;
	}

	u8 read_address() const {
		return m_address | 0x80;
	}

	u8 write_address() const {
		return m_address & 0x7f;
	}

	u8 interface() const {
		return m_interface;
	}

	u16 max_packet_size() const {
		return m_max_packet_size;
	}

private:
	enum EndpointDescriptor::transfer_type m_transfer_type;
	u8 m_address;
	u8 m_interface;
	u16 m_max_packet_size;
};

using EndpointList = var::Vector<Endpoint>;

class Device;

class DeviceHandle : public fs::File {
public:
	DeviceHandle(){}
	DeviceHandle(libusb_device_handle * handle){
		m_handle = handle;
	}

	DeviceHandle& set_device(Device * device){
		m_device = device;
		return *this;
	}

	int open(
			const var::String & name,
			const fs::OpenFlags & flags = fs::OpenFlags::read_write()
			){
		if( is_valid() ){
			load_endpoint_list();
			return 0;
		}
		return -1;
	}

	u32 size() const {
		return 0;
	}

	int close(){
		if( m_handle ){
			libusb_device_handle * handle = m_handle;
			m_handle = nullptr;
			libusb_close(m_handle);
		}
		return 0;
	}

	int read(
			void * buf,
			Size size
			) const;

	int write(
			const void * buf,
			Size size
			) const;

	int seek(
			int location,
			enum whence whence = SET
			) const {
		m_location = location;
		return m_location;
	}

	int ioctl(
			IoRequest request,
			IoArgument arg
			) const {
		//not supported
		return -1;
	}

	bool is_valid() const {
		return m_handle != nullptr;
	}

	int get_configuration();
	int set_configuration(int configuration_number);
	int claim_interface(int interface_number);
	int release_interface(int interface_number);
	int set_interface_alternate_setting(
			int interface_number,
			int alternate_setting
			);

	int clear_halt(u8 endpoint_address){
		return libusb_clear_halt(m_handle, endpoint_address);
	}

	int reset(){
		return libusb_reset_device(m_handle);
	}

	bool is_kernel_driver_active(int interface_number){
		return libusb_kernel_driver_active(
					m_handle,
					interface_number
					);
	}

	int attach_kernel_driver(int interface_number){
		return libusb_attach_kernel_driver(
					m_handle,
					interface_number
					);
	}

	int detach_kernel_driver(int interface_number){
		return libusb_detach_kernel_driver(
					m_handle,
					interface_number
					);
	}

	int set_auto_detach_kernel_driver(bool value = true){
		return libusb_set_auto_detach_kernel_driver (
					m_handle,
					value
					);
	}

	void set_timeout(const chrono::MicroTime & duration){
		m_timeout = duration;
	}

private:
	libusb_device_handle * m_handle;
	mutable u8 m_location;
	EndpointList m_endpoint_list;
	Device * m_device;
	chrono::MicroTime m_timeout;

	const Endpoint find_endpoint(u8 address) const;
	void load_endpoint_list();
	int transfer(const Endpoint & endpoint, void * buf, int nbyte, bool is_read) const;
	int transfer_packet(const Endpoint & endpoint, void * buf, int nbyte, bool is_read) const;

};

class Device {
public:

	Device(libusb_device * device);

	bool is_valid() const {
		return m_device != nullptr;
	}

	DeviceHandle get_handle(){
		libusb_device_handle * handle;
		if( libusb_open(m_device, &handle) < 0){
			return DeviceHandle();
		}
		return DeviceHandle(handle).set_device(this);
	}

	u8 get_bus_number() const {
		return libusb_get_bus_number(m_device);
	}

	u8 get_port_number() const {
		return libusb_get_port_number(m_device);
	}

	var::Vector<u8> get_port_numbers() const {
		var::Vector<u8> result(7);
		if( libusb_get_port_numbers(m_device, result.data(), result.count()) < 0 ){
			return var::Vector<u8>();
		}
		return result;
	}

	Device get_parent() const {
		return Device(libusb_get_parent(m_device));
	}

	u8 get_device_addres() const {
		return libusb_get_device_address(m_device);
	}

	DeviceDescriptor get_device_descriptor() const;

	ConfigurationDescriptor get_configuration_descriptor(
			int configuration_number
			) const;

	ConfigurationDescriptor get_active_configuration_descriptor() const;

	ConfigurationDescriptorList configuration_list() const {
		ConfigurationDescriptorList result;
		size_t configuration_count = get_device_descriptor().configuration_count();

		for(size_t i=0; i < configuration_count; i++){
			result.push_back(
						get_configuration_descriptor(i)
						);
		}

		return result;
	}

	const var::Vector<var::String> string_list() const {
		return m_string_list;
	}

private:

	libusb_device * m_device = nullptr;
	var::Vector<var::String> m_string_list;

	void load_strings();

};

class DeviceList : public DeviceFlags, public var::Vector<Device> {
public:

	Device * find(
			VendorId vendor_id,
			ProductId product_id,
			const var::String & serial_number = var::String()
			);


private:

};

}

#endif // USBAPI_DEVICE_HPP
