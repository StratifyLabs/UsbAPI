// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include <json.hpp>

#include "usb/Descriptor.hpp"

using namespace usb;

DeviceDescriptor::DeviceDescriptor(
  const libusb_device_descriptor &value,
  const DescriptorStringList &string_list)
  : Descriptor(&m_device_descriptor, string_list) {
  m_device_descriptor = value;
}

json::JsonObject DeviceDescriptor::to_object() const {
  json::JsonObject result;
  result.insert("bLength", json::JsonInteger(length()));
  result.insert("bDescriptorType", json::JsonInteger(type()));
  result.insert("bcdUSB", json::JsonInteger(bcd_usb()));
  result.insert("bcdDevice", json::JsonInteger(bcd_device()));
  result.insert("bDeviceClass", json::JsonInteger(device_class()));
  result.insert("bDeviceSubClass", json::JsonInteger(device_sub_class()));
  result.insert("bDeviceProtocol", json::JsonInteger(device_protocol()));
  result.insert("bMaxPacketSize0", json::JsonInteger(max_control_packet_size()));
  result.insert("idVendor", json::JsonInteger(vendor_id()));
  result.insert("idProduct", json::JsonInteger(product_id()));
  result.insert("iManufacturer", json::JsonString(manufacturer_string()));
  result.insert("iProduct", json::JsonString(product_string()));
  result.insert("iSerialNumber", json::JsonString(serial_number_string()));
  result.insert("bNumConfigurations", json::JsonInteger(configuration_count()));
  return result;
}

json::JsonObject EndpointDescriptor::to_object() const {
  json::JsonObject result;
  result.insert("bLength", json::JsonInteger(length()));
  result.insert("bDescriptorType", json::JsonInteger(type()));
  result.insert("bEndpointAddress", json::JsonInteger(endpoint_address()));
  result.insert("bmAttributes", json::JsonInteger(attributes()));
  result.insert("wMaxPacketSize", json::JsonInteger(max_packet_size()));
  result.insert("bInterval", json::JsonInteger(interval()));
  result.insert("bRefresh", json::JsonInteger(refresh()));
  result.insert("bSynchAddress", json::JsonInteger(synch_address()));
  return result;
}

json::JsonObject InterfaceDescriptor::to_object() const {
  json::JsonObject result;
  result.insert("bLength", json::JsonInteger(length()));
  result.insert("bDescriptorType", json::JsonInteger(type()));
  result.insert("bInterfaceNumber", json::JsonInteger(interface_number()));
  result.insert("bAlternateSetting", json::JsonInteger(alternate_setting()));
  result.insert("bInterfaceClass", json::JsonInteger(interface_class()));
  result.insert("bInterfaceSubClass", json::JsonInteger(interface_sub_class()));
  result.insert("bInterfaceProtocol", json::JsonInteger(interface_protocol()));
  result.insert("bNumEndpoints", json::JsonInteger(endpoint_count()));
  result.insert("iInterface", json::JsonString(interface_string()));
  result.insert("endpointsList", json::JsonArray(endpoint_list()));
  return result;
}

json::JsonObject Interface::to_object() const {
  json::JsonObject result;
  result.insert(
    "bNumAlternateSettings",
    json::JsonInteger(alternate_settings_count()));
  result.insert(
    "alternateSettingsList",
    json::JsonArray(alternate_settings_list()));
  return result;
}

json::JsonObject ConfigurationDescriptor::to_object() const {
  json::JsonObject result;
  result.insert("bLength", json::JsonInteger(length()));
  result.insert("bDescriptorType", json::JsonInteger(type()));
  result.insert("wTotalLength", json::JsonInteger(total_length()));
  result.insert("bNumInterfaces", json::JsonInteger(interface_count()));
  result.insert("bConfigurationValue", json::JsonInteger(configuration_value()));
  result.insert("iConfiguration", json::JsonString(configuration_string()));
  result.insert("bmAttributes", json::JsonInteger(attributes()));
  result.insert("MaxPower", json::JsonInteger(max_power()));
  result.insert("interfaceList", json::JsonArray(interface_list()));
  return result;
}
