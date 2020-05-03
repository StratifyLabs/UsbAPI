#include "Session.hpp"

using namespace usb;

Session::Session(){
	libusb_init(&m_context);

	 //libusb_set_debug(m_context, LIBUSB_LOG_LEVEL_INFO);
}
