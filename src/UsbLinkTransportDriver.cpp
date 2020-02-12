#include "UsbLinkTransportDriver.hpp"

UsbLinkTransportDriver::UsbLinkTransportDriver(){}

int UsbLinkTransportDriver::initialize(
		const UsbLinkTransportDriverOptions & options
		){

	//find a device in the list that matches options
	m_device_list = m_session.get_device_list();

	usb::Device * device = m_device_list.find(
				usb::DeviceList::VendorId(options.vendor_id()),
				usb::DeviceList::ProductId(options.product_id()),
				options.serial_number()
				);

	if( device == nullptr ){
		return -1;
	}

	m_device_handle = device->get_handle();

	if( m_device_handle.open(
				""
				) < 0){
		return -1;
	}

	m_device_handle.set_timeout( options.timeout() );

	return 0;
}

int UsbLinkTransportDriver::finalize(){
	m_device_handle.close();
	return 0;
}
