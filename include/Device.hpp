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

class DeviceHandle : public fs::File {
public:

	DeviceHandle(){}

	DeviceHandle(libusb_device_handle * handle){
		m_handle = handle;
	}

	int open(
			const var::String & name,
			const fs::OpenFlags & flags = fs::OpenFlags::read_write()
			){
		if( is_valid() ){
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


private:
	libusb_device_handle * m_handle;
	mutable u8 m_location;
};

class Device
{
public:
	Device(libusb_device * device);

	bool is_valid() const {
		return m_device != nullptr;
	}

	DeviceHandle open(){
		libusb_device_handle * handle;
		if( libusb_open(m_device, &handle) < 0){
			return DeviceHandle();
		}
		return handle;
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

	Device get_parent(){
		return Device(libusb_get_parent(m_device));
	}

	u8 get_device_addres(){
		return libusb_get_device_address(m_device);
	}

	DeviceDescriptor get_device_descriptor();

	ConfigurationDescriptor get_configuration_descriptor(
			int configuration_number
			);

	ConfigurationDescriptor get_active_configuration_descriptor();

private:
	libusb_device * m_device = nullptr;
};

class DeviceList : public DeviceFlags{
public:

	Device & at(size_t offset){
		return m_list.at(offset);
	}

	const Device & at(size_t offset) const {
		return m_list.at(offset);
	}

	Device & find(
			VendorId vendor_id,
			ProductId product_id
			);

	void push_back(const Device & device){
		m_list.push_back(device);
	}

private:
	var::Vector<Device> m_list;

};

}

#endif // USBAPI_DEVICE_HPP
