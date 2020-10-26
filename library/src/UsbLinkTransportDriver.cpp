#include "UsbLinkTransportDriver.hpp"

usb::Session UsbLinkTransportDriver::m_session;

UsbLinkTransportDriver::UsbLinkTransportDriver() : m_device_handle() {}

int UsbLinkTransportDriver::initialize(
  const UsbLinkTransportDriverOptions &options) {

  m_options = options;

  usb::Device *device
    = session().device_list()(usb::DeviceList::Find()
                                .set_vendor_id(options.vendor_id())
                                .set_product_id(options.product_id())
                                .set_serial_number(options.serial_number()));

  if (device == nullptr) {
    // try re-loading the list if nothing was found
    device = reload_list_and_find_device(options);
  }

  if (device == nullptr) {
    printf("%s():%d\n", __FUNCTION__, __LINE__);
    return -1;
  }

  printf("%s():%d\n", __FUNCTION__, __LINE__);
  m_device_handle = device->get_handle(1, options.interface_path());

  if (m_device_handle.is_valid() == false) {
    device = reload_list_and_find_device(options);
    if (device == nullptr) {
      printf("%s():%d\n", __FUNCTION__, __LINE__);
      return -1;
    }

    printf("%s():%d\n", __FUNCTION__, __LINE__);
    m_device_handle = device->get_handle(1, options.interface_path());
    if (m_device_handle.is_valid() == false) {
      printf("%s():%d\n", __FUNCTION__, __LINE__);
      return -1;
    }
  }

  printf("%s():%d\n", __FUNCTION__, __LINE__);
  m_device_handle.set_timeout(options.timeout());
  printf(
    "%s():%d %d\n",
    __FUNCTION__,
    __LINE__,
    m_device_handle.endpoint_list().count());

  return 0;
}

int UsbLinkTransportDriver::finalize() {
  // m_device_handle.close();
  return 0;
}

int UsbLinkTransportDriver::get_status() {

  // is device still available?
  return 0;
}

usb::Device *UsbLinkTransportDriver::reload_list_and_find_device(
  const UsbLinkTransportDriverOptions &options) {
  // try re-loading the list if nothing was found
  session().get_device_list(usb::SessionOptions()
                              .set_vendor_id(options.vendor_id())
                              .set_product_id(options.product_id()));
  return session().device_list()(usb::DeviceList::Find()
                                   .set_product_id(options.product_id())
                                   .set_vendor_id(options.vendor_id())
                                   .set_serial_number(options.serial_number()));
}
