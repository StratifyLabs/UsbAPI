#ifndef USBLINKTRANSPORTDRIVER_HPP
#define USBLINKTRANSPORTDRIVER_HPP

#include <sapi/var/String.hpp>
#include "usb_link_transport_driver.h"
#include "Session.hpp"

class UsbLinkTransportDriverOptions {
public:

	UsbLinkTransportDriverOptions(){
		m_is_usb_path = false;
	}

	UsbLinkTransportDriverOptions(
			const var::String & path
			){
		if( path.find("usb@") == 0 ){
			var::Vector<var::String> path_items = path.split("/");
			if( path_items.count() < 3 ){
				return;
			}

			m_vendor_id = path_items.at(1);
			m_product_id= path_items.at(2);
			if( path_items.count() == 4){
				m_serial_number = path_items.at(3);
			} else if( path_items.count() > 4 ){
				return;
			}
			m_is_usb_path = true;

		} else {
			m_is_usb_path = false;
		}
	}

	bool is_valid() const {
		return m_is_usb_path;
	}

	u16 vendor_id() const {
		return m_vendor_id.to_unsigned_long(var::String::base_16);
	}

	u16 product_id() const {
		return m_product_id.to_unsigned_long(var::String::base_16);
	}

	const var::String & serial_number() const {
		return m_serial_number;
	}

	chrono::Milliseconds timeout() const {
		return chrono::Milliseconds(1);
	}

private:
	bool m_is_usb_path;
	var::String m_vendor_id;
	var::String m_product_id;
	var::String m_serial_number;

};

class UsbLinkTransportDriver
{
public:
	UsbLinkTransportDriver();

	int initialize(const UsbLinkTransportDriverOptions & options);
	int finalize();

	int get_status();

	const usb::DeviceHandle & device_handle() const {
		return m_device_handle;
	}

	static var::String build_usb_path(u16 vendor_id, u16 product_id, const var::String & serial_number){
		return var::String().format("usb@/%04X/%04X/", vendor_id, product_id) + serial_number;
	}

	static var::String build_usb_path(const usb::Device & device){
		const usb::DeviceDescriptor device_descriptor = device.get_device_descriptor();

		return build_usb_path(
					device_descriptor.vendor_id(),
					device_descriptor.product_id(),
					device_descriptor.serial_number_string()
					);
	}

	static bool is_device_stratify_os(const usb::Device & device){
		for(const auto & entry: device.string_list()){
			var::String lower_entry = entry;
			lower_entry.to_lower();
			if( lower_entry.find("stratify") != var::String::npos ){
				return true;
			}
		}
		return false;
	}

private:
	usb::Session m_session;
	usb::DeviceList m_device_list;
	usb::DeviceHandle m_device_handle;
	u8 m_endpoint_address;
	UsbLinkTransportDriverOptions m_options;
};

#endif // USBLINKTRANSPORTDRIVER_HPP
