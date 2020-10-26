#ifndef USBLINKTRANSPORTDRIVER_HPP
#define USBLINKTRANSPORTDRIVER_HPP

#include <var/String.hpp>

#include "usb/Session.hpp"
#include "usb/usb_link_transport_driver.h"

class UsbLinkPath {
public:
  UsbLinkPath(const usb::Device &device, u16 interface_number) {
    const usb::DeviceDescriptor device_descriptor
      = device.get_device_descriptor();
    m_vendor_id = device_descriptor.vendor_id();
    m_product_id = device_descriptor.product_id();
    m_interface_number = interface_number;
    m_serial_number = device_descriptor.serial_number_string();
  }

  bool is_valid() const { return vendor_id() != 0; }

  var::String build_path() const {
    return var::String().format(
             "/usb/%04X/%04X/%02X/",
             vendor_id(),
             product_id(),
             interface_number())
           + serial_number();
  }

private:
  API_ACCESS_FUNDAMENTAL(UsbLinkPath, u16, vendor_id, 0);
  API_ACCESS_FUNDAMENTAL(UsbLinkPath, u16, product_id, 0);
  API_ACCESS_FUNDAMENTAL(UsbLinkPath, u16, interface_number, 0);
  API_READ_ACCESS_COMPOUND(UsbLinkPath, var::String, serial_number);
};

class UsbLinkTransportDriverOptions : api::ExecutionContext {
public:
  UsbLinkTransportDriverOptions() { m_is_usb_path = false; }

  UsbLinkTransportDriverOptions(const var::StringView path) {

    auto list = path.split("/");

    printf("count is %d\n", list.count());
    if (list.count() > 1) {
      if (list.at(1) != "usb") {
        m_is_usb_path = false;
        return;
      }
    }

    if (list.count() > 2) {
      m_vendor_id = list.at(2);
    }

    if (list.count() > 3) {
      m_product_id = list.at(3);
    }

    if (list.count() > 4) {
      m_interface_path = list.at(4);
    }

    if (list.count() > 5) {
      m_serial_number = list.at(5);
    }

    m_is_usb_path = true;
  }

  bool is_valid() const { return m_is_usb_path; }

  u16 vendor_id() const {
    return m_vendor_id.to_unsigned_long(var::StringView::Base::hexidecimal);
  }

  u16 product_id() const {
    return m_product_id.to_unsigned_long(var::StringView::Base::hexidecimal);
  }

  u16 interface_number() const {
    return m_interface_path.to_unsigned_long(
      var::StringView::Base::hexidecimal);
  }

  const var::StringView &interface_path() const { return m_interface_path; }

  const var::StringView &serial_number() const { return m_serial_number; }

  chrono::MicroTime timeout() const { return 10_milliseconds; }

private:
  bool m_is_usb_path;
  var::StringView m_interface_path;
  var::StringView m_vendor_id;
  var::StringView m_product_id;
  var::StringView m_serial_number;
};

class UsbLinkTransportDriver {
public:
  UsbLinkTransportDriver();

  int initialize(const UsbLinkTransportDriverOptions &options);
  int finalize();

  int get_status();

  static bool is_device_stratify_os(const usb::Device &device) {

    for (const auto &entry : device.string_list()) {
      if (
        var::String(entry).to_lower().string_view().find("stratify")
        != var::String::npos) {
        return true;
      }
    }

    // if there are no strings -- check the interface
    return true;
  }

  static bool is_interface_stratify_os(
    const usb::InterfaceDescriptor &interface_descriptor) {
    if (
      (interface_descriptor.interface_class() == 0xff)
      && (interface_descriptor.interface_sub_class() == 0x50)
      && (interface_descriptor.interface_protocol() == 0x51)) {
      return true;
    }

    return var::String(interface_descriptor.interface_string())
             .to_lower()
             .string_view()
             .find("stratify")
           != var::String::npos;
  }

  static usb::Session &session() { return m_session; }

  const usb::DeviceHandle &device_handle() const { return m_device_handle; }
  usb::DeviceHandle &device_handle() { return m_device_handle; }

private:
  API_ACCESS_FUNDAMENTAL(UsbLinkTransportDriver, u8, endpoint_address, 0xff);
  usb::DeviceHandle m_device_handle;
  static usb::Session m_session;
  UsbLinkTransportDriverOptions m_options;

  usb::Device *
  reload_list_and_find_device(const UsbLinkTransportDriverOptions &options);
};

#endif // USBLINKTRANSPORTDRIVER_HPP
