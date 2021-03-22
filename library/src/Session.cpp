// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include "usb/Session.hpp"

using namespace usb;

#define LIBUSB_VERBOSE_DEBUG 0


Session::Session() {
  libusb_init(&m_context);

#if LIBUSB_VERBOSE_DEBUG
  libusb_set_option(m_context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
#endif
}
