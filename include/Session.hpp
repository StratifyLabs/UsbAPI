#ifndef USBAPI_SESSION_HPP
#define USBAPI_SESSION_HPP

#include "Device.hpp"

namespace usb {

class Session : public DeviceFlags {
public:

	Session();
	~Session(){
		libusb_exit(m_context);
		m_context = nullptr;
	}

	DeviceList get_device_list(){
		DeviceList result;
		libusb_device ** device;
		ssize_t	count = libusb_get_device_list(
					m_context,
					&device
					);

		for(ssize_t i=0; i < count; i++){
			result.push_back(
						Device(device[i])
						);
		}

		//free list?

		return result;
	}

private:
	libusb_context * m_context = nullptr;
};

}

#endif // USBAPI_SESSION_HPP
