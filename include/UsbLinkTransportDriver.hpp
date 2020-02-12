#ifndef USBLINKTRANSPORTDRIVER_HPP
#define USBLINKTRANSPORTDRIVER_HPP

#include "usb_link_transport_driver.h"
#include "Session.hpp"

class UsbLinkTransportDriverOptions {
public:

private:
	usb_link_transport_driver_options_t m_options;
};

class UsbLinkTransportDriver
{
public:
	UsbLinkTransportDriver();


private:
	usb::Session m_session;
	usb::DeviceList m_device_list;
	usb::DeviceHandle m_device_handle;
};

#endif // USBLINKTRANSPORTDRIVER_HPP
