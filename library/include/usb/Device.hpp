#ifndef USBAPI_DEVICE_HPP
#define USBAPI_DEVICE_HPP

#include <fs/File.hpp>
#include <var/Vector.hpp>

#include "Descriptor.hpp"

namespace usb {

class Endpoint : public UsbFlags {
public:
  Endpoint() {
    m_transfer_type = TransferType::none;
    m_address = 0;
    m_interface = 0;
    m_max_packet_size = 0;
  }

  Endpoint(const EndpointDescriptor &endpoint_descriptor) {
    m_transfer_type = endpoint_descriptor.transfer_type();
    m_address = endpoint_descriptor.endpoint_address() & 0x7f;
    m_max_packet_size = endpoint_descriptor.max_packet_size();
    m_interface = 0;
  }

  bool is_valid() const { return m_transfer_type != TransferType::none; }

  Endpoint &set_transfer_type(TransferType value) {
    m_transfer_type = value;
    return *this;
  }

  Endpoint &set_address(u8 value) {
    m_address = value & 0x7f;
    return *this;
  }

  Endpoint &set_interface(u8 value) {
    m_interface = value & 0x7f;
    return *this;
  }

  TransferType transfer_type() const { return m_transfer_type; }

  u8 address() const { return m_address; }

  u8 read_address() const { return m_address | 0x80; }

  u8 write_address() const { return m_address & 0x7f; }

  u8 interface() const { return m_interface; }

  u16 max_packet_size() const { return m_max_packet_size; }

  static const Endpoint &empty() { return m_empty_endpoint; }

private:
  TransferType m_transfer_type;
  u8 m_address;
  u8 m_interface;
  u16 m_max_packet_size;
  static Endpoint m_empty_endpoint;
};

using EndpointList = var::Vector<Endpoint>;

class Device;

class DeviceHandle : public fs::FileAccess<DeviceHandle>, public UsbFlags {
public:
  DeviceHandle() {}

  DeviceHandle(
    libusb_device_handle *handle,
    int configuration,
    const var::StringView name) {
    m_handle = handle;
    set_configuration(configuration);
    open(name, fs::OpenMode::read_write());
  }

  DeviceHandle &set_device(Device *device) {
    m_device = device;
    return *this;
  }

  DeviceHandle(DeviceHandle &&a) { std::swap(m_handle, a.m_handle); }
  DeviceHandle &operator=(DeviceHandle &&a) {
    std::swap(m_handle, a.m_handle);
    return *this;
  }

  ~DeviceHandle() { close(); }

  bool is_valid() const { return m_handle != nullptr; }

  u32 size() const { return 0; }

  int get_configuration() {
    API_RETURN_VALUE_IF_ERROR(-1);
    int configuration_number;
    API_SYSTEM_CALL(
      "",
      libusb_get_configuration(m_handle, &configuration_number));
    return configuration_number;
  }

  DeviceHandle &set_configuration(int configuration_number) {
    API_RETURN_VALUE_IF_ERROR(*this);
    API_SYSTEM_CALL(
      "",
      libusb_set_configuration(m_handle, configuration_number));
    return *this;
  }

  DeviceHandle &claim_interface() {
    API_RETURN_VALUE_IF_ERROR(*this);
    API_SYSTEM_CALL("", libusb_claim_interface(m_handle, m_interface_number));
    return *this;
  }

  DeviceHandle &release_interface() {
    API_RETURN_VALUE_IF_ERROR(*this);
    API_SYSTEM_CALL("", libusb_release_interface(m_handle, m_interface_number));
    return *this;
  }

  DeviceHandle &
  set_interface_alternate_setting(int interface_number, int alternate_setting) {

    API_RETURN_VALUE_IF_ERROR(*this);
    API_SYSTEM_CALL(
      "",
      libusb_set_interface_alt_setting(
        m_handle,
        interface_number,
        alternate_setting));
    return *this;
  }

  DeviceHandle &clear_halt(u8 endpoint_address) {
    API_RETURN_VALUE_IF_ERROR(*this);
    API_SYSTEM_CALL("", libusb_clear_halt(m_handle, endpoint_address));
    return *this;
  }

  DeviceHandle &reset() {
    API_RETURN_VALUE_IF_ERROR(*this);
    API_SYSTEM_CALL("", libusb_reset_device(m_handle));
    return *this;
  }

  bool is_kernel_driver_active(int interface_number) {
    API_RETURN_VALUE_IF_ERROR(false);
    int result = API_SYSTEM_CALL(
      "",
      libusb_kernel_driver_active(m_handle, interface_number));
    return result > 0;
  }

  DeviceHandle &attach_kernel_driver(int interface_number) {
    API_RETURN_VALUE_IF_ERROR(*this);
    API_SYSTEM_CALL(
      "",
      libusb_attach_kernel_driver(m_handle, interface_number));
    return *this;
  }

  DeviceHandle &detach_kernel_driver(int interface_number) {
    API_RETURN_VALUE_IF_ERROR(*this);
    API_SYSTEM_CALL(
      "",
      libusb_detach_kernel_driver(m_handle, interface_number));
    return *this;
  }

  DeviceHandle &set_auto_detach_kernel_driver(bool value = true) {
    API_RETURN_VALUE_IF_ERROR(*this);
    API_SYSTEM_CALL("", libusb_set_auto_detach_kernel_driver(m_handle, value));
    return *this;
  }

private:
  class DeviceReadBuffer {
  public:
    int copy_and_erase_bytes(void *dest, int nbyte) {
      int byte_count = nbyte < buffer().size() ? nbyte : buffer().size();
      if (byte_count) {
        memcpy(dest, buffer().data(), byte_count);
        buffer()(var::Data::Erase().set_size(byte_count));
      }
      return byte_count;
    }

  private:
    API_ACCESS_COMPOUND(DeviceReadBuffer, var::Data, buffer);
    API_ACCESS_FUNDAMENTAL(DeviceReadBuffer, u8, address, 0xff);
  };

  mutable u8 m_location;
  int m_interface_number;
  API_READ_ACCESS_COMPOUND(DeviceHandle, EndpointList, endpoint_list);
  API_ACCESS_COMPOUND(DeviceHandle, chrono::MicroTime, timeout);
  mutable var::Vector<DeviceReadBuffer> m_read_buffer_list;
  libusb_device_handle *m_handle = nullptr;
  Device *m_device = nullptr;

  int interface_lseek(int offset, int whence) const override final {
    switch (static_cast<Whence>(whence)) {
    case Whence::current:
      m_location += offset;
      break;
    case Whence::set:
      m_location = offset;
      break;
    case Whence::end:
      return -1;
    }
    return m_location;
  }

  int interface_read(void *buf, int nbyte) const override final;
  int interface_write(const void *buf, int nbyte) const override final;

  int interface_ioctl(int request, void *argument) const override final {
    return fake_ioctl(request, argument);
  }

  void open(
    const var::StringView name,
    const fs::OpenMode &flags = fs::OpenMode::read_write()) {

    if (is_valid()) {
      m_interface_number
        = name.to_unsigned_long(var::StringView::Base::hexidecimal);
      load_endpoint_list();
      claim_interface();
      return;
    }
    m_interface_number = -1;
  }

  void close() {
    if (m_handle) {
      libusb_device_handle *handle = m_handle;
      release_interface();
      m_handle = nullptr;
      libusb_close(handle);
    }
  }

  const Endpoint &find_endpoint(u8 address) const;
  void load_endpoint_list();
  int transfer(const Endpoint &endpoint, void *buf, int nbyte, bool is_read)
    const;
  int transfer_packet(
    const Endpoint &endpoint,
    void *buf,
    int nbyte,
    bool is_read) const;
};

class Device : public api::ExecutionContext, public UsbFlags {
public:
  Device(libusb_device *device);

  bool is_valid() const { return m_device != nullptr; }

  DeviceHandle get_handle(int configuration, const var::StringView path) {
    if (is_error()) {
      return std::move(DeviceHandle());
    }
    load_strings();
    libusb_device_handle *handle = nullptr;
    int result;
    API_SYSTEM_CALL("", libusb_open(m_device, &handle));
    if (is_error()) {
      return std::move(DeviceHandle());
    }
    return std::move(
      DeviceHandle(handle, configuration, path).set_device(this));
  }

  u8 get_bus_number() const {
    API_RETURN_VALUE_IF_ERROR(0);
    return API_SYSTEM_CALL("", libusb_get_bus_number(m_device));
  }

  u8 get_port_number() const {
    API_RETURN_VALUE_IF_ERROR(0);
    return API_SYSTEM_CALL("", libusb_get_port_number(m_device));
  }

  var::Vector<u8> get_port_numbers() const {
    var::Vector<u8> result(7);
    if (libusb_get_port_numbers(m_device, result.data(), result.count()) < 0) {
      return var::Vector<u8>();
    }
    return result;
  }

  Device get_parent() const {
    API_RETURN_VALUE_IF_ERROR(0);
    return Device(libusb_get_parent(m_device));
  }

  u8 get_device_address() const {
    API_RETURN_VALUE_IF_ERROR(0);
    return API_SYSTEM_CALL("", libusb_get_device_address(m_device));
  }

  DeviceDescriptor get_device_descriptor() const;

  ConfigurationDescriptor
  get_configuration_descriptor(int configuration_number) const;

  ConfigurationDescriptor get_active_configuration_descriptor() const;

  ConfigurationDescriptorList configuration_list() const {
    ConfigurationDescriptorList result;
    size_t configuration_count = get_device_descriptor().configuration_count();

    for (size_t i = 0; i < configuration_count; i++) {
      result.push_back(get_configuration_descriptor(i));
    }

    return result;
  }

private:
  libusb_device *m_device = nullptr;
  API_READ_ACCESS_COMPOUND(Device, var::StringList, string_list);

  void load_strings();
};

class DeviceList : public UsbFlags, public var::Vector<Device> {
public:
  class Find {
    API_AF(Find, u16, vendor_id, 0);
    API_AF(Find, u16, product_id, 0);
    API_AC(Find, var::StringView, serial_number);
  };

  Device *find(const Find &options);
  inline Device *operator()(const Find &options) { return find(options); }

private:
};

} // namespace usb

#endif // USBAPI_DEVICE_HPP
