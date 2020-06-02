#include "UsbLinkTransportDriver.hpp"

UsbLinkTransportDriver::UsbLinkTransportDriver(){}

int UsbLinkTransportDriver::initialize(
		const UsbLinkTransportDriverOptions & options
		){

	m_options = options;
	//find a device in the list that matches options
	m_device_list = m_session.get_device_list(
				usb::SessionOptions()
				.set_vendor_id(options.vendor_id())
				.set_product_id(options.product_id())
				);

	usb::Device * device = m_device_list.find(
				usb::DeviceList::VendorId(options.vendor_id()),
				usb::DeviceList::ProductId(options.product_id()),
				options.serial_number()
				);

	if( device == nullptr ){
		return -1;
	}

	m_device_handle = device->get_handle();
	if( m_device_handle.set_configuration(1) < 0 ){
		return -1;
	}

	if( m_device_handle.open(
				options.interface_path()
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

int UsbLinkTransportDriver::get_status(){

	//is device still available?
	return 0;
}

