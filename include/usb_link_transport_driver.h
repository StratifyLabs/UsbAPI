#ifndef USB_LINK_TRANSPORT_DRIVER_H
#define USB_LINK_TRANSPORT_DRIVER_H

#include <sos/link/transport.h>

#if defined __cplusplus
extern "C" {
#endif

void usb_link_transport_load_driver(
		link_transport_mdriver_t * driver
		);

link_transport_phy_t usb_link_transport_driver_open(const char * path, const void * options);
int usb_link_transport_driver_write(link_transport_phy_t handle, const void * buffer, int nbyte);
int usb_link_transport_driver_read(link_transport_phy_t handle, void * buffer, int nbyte);
int usb_link_transport_driver_close(link_transport_phy_t * handle);
void usb_link_transport_driver_wait(int milliseconds);
void usb_link_transport_driver_flush(link_transport_phy_t handle);
void usb_link_transport_driver_request(link_transport_phy_t handle);

int usb_link_transport_getname(char * dest, const char * last, int len);
int usb_link_transport_lock(link_transport_phy_t handle);
int usb_link_transport_unlock(link_transport_phy_t handle);
int usb_link_transport_status(link_transport_phy_t handle);

#if defined __cplusplus
}
#endif

#endif // USB_LINK_TRANSPORT_DRIVER_H
