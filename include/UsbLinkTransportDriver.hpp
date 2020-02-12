#ifndef USBLINKTRANSPORTDRIVER_HPP
#define USBLINKTRANSPORTDRIVER_HPP

#include "usb_link_transport_driver.h"
#include "Session.hpp"

class UsbLinkTransportDriverOptions {
public:

	UsbLinkTransportDriverOptions(
			const usb_link_transport_driver_options_t * options
			){
		if( options != nullptr ){
			m_options = *options;
		}
	}

	u16 vendor_id() const {
		return m_options.vendor_id;
	}

	u16 product_id() const {
		return m_options.product_id;
	}

	const var::String serial_number() const {
		return var::String(m_options.serial_number);
	}

	chrono::Milliseconds timeout() const {
		return chrono::Milliseconds(m_options.timeout_milliseconds);
	}

private:
	usb_link_transport_driver_options_t m_options;
};

class UsbLinkTransportDriver
{
public:
	UsbLinkTransportDriver();

	int initialize(const UsbLinkTransportDriverOptions & options);
	int finalize();

	const usb::DeviceHandle & device_handle() const {
		return m_device_handle;
	}

private:
	usb::Session m_session;
	usb::DeviceList m_device_list;
	usb::DeviceHandle m_device_handle;
	u8 m_endpoint_address;
};

#endif // USBLINKTRANSPORTDRIVER_HPP
