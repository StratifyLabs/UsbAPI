#ifndef USBAPI_SESSION_HPP
#define USBAPI_SESSION_HPP

#include "Device.hpp"

namespace usb {

class SessionOptions {
public:

	bool is_all() const {
		return vendor_id() + product_id() + device_class() + device_sub_class() == 0;
	}

private:
	API_ACCESS_FUNDAMENTAL(SessionOptions,u16,vendor_id,0);
	API_ACCESS_FUNDAMENTAL(SessionOptions,u16,product_id,0);
	API_ACCESS_FUNDAMENTAL(SessionOptions,u16,device_class,0);
	API_ACCESS_FUNDAMENTAL(SessionOptions,u16,device_sub_class,0);
};

class Session : public DeviceFlags {
public:

	Session();
	~Session(){
		libusb_exit(m_context);
		m_context = nullptr;
	}

	void reinitialize(){
		libusb_exit(m_context);
		m_context = nullptr;
		libusb_init(&m_context);
	}

	DeviceList get_device_list(
			const SessionOptions& options
			){
		DeviceList result;
		libusb_device ** device;
		ssize_t	count = libusb_get_device_list(
					m_context,
					&device
					);
		result.reserve(count);
		for(ssize_t i=0; i < count; i++){
			libusb_device_descriptor desc;
			bool is_match = true;
			if( options.is_all() == false ){
				libusb_get_device_descriptor(device[i], &desc);
				if( options.vendor_id() ){
					if( desc.idVendor != options.vendor_id() ){
						is_match = false;
					}
				}

				if( is_match && options.product_id() ){
					if( desc.idProduct != options.product_id() ){
						is_match = false;
					}
				}
			}

			if( is_match ){
				result.push_back(
							Device(device[i])
							);
			}
		}

		return result;
	}

private:
	libusb_context * m_context = nullptr;
};

}

#endif // USBAPI_SESSION_HPP
