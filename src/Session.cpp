#include "Session.hpp"

using namespace usb;

Session::Session(){
	libusb_init(&m_context);
}
