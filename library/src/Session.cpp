// Copyright 2020-2021 Tyler Gilbert and Stratify Labs, Inc; see LICENSE.md

#include "usb/Session.hpp"

using namespace usb;

Session::Session(){
	libusb_init(&m_context);

	 //libusb_set_option(m_context, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
}
