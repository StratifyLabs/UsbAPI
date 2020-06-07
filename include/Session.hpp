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
		free_device_list();
		free_context();
	}

	void reinitialize(){
		free_device_list();
		free_context();
		libusb_init(&m_context);
	}

	const DeviceList & get_device_list(
			const SessionOptions& options
			){

		free_device_list();

		if( m_libusb_device_list != nullptr ){
			libusb_free_device_list(m_libusb_device_list, 1);
			m_libusb_device_list = nullptr;
		}

		ssize_t	count = libusb_get_device_list(
					m_context,
					&m_libusb_device_list
					);
		m_device_list.clear();
		m_device_list.reserve(count);
		for(ssize_t i=0; i < count; i++){
			libusb_device_descriptor desc;
			bool is_match = true;
			if( options.is_all() == false ){
				libusb_get_device_descriptor(m_libusb_device_list[i], &desc);
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
				m_device_list.push_back(
							Device(m_libusb_device_list[i])
							);
			}
		}

		return device_list();
	}

private:
	API_ACCESS_COMPOUND(Session,DeviceList,device_list);
	libusb_context * m_context = nullptr;
	libusb_device ** m_libusb_device_list = nullptr;

	void free_device_list(){
		if( m_libusb_device_list != nullptr ){
			libusb_free_device_list(m_libusb_device_list, 1);
			m_libusb_device_list = nullptr;
		}
	}

	void free_context(){
		if( m_context != nullptr ){
			libusb_exit(m_context);
			m_context = nullptr;
		}
	}
};

}

#endif // USBAPI_SESSION_HPP
