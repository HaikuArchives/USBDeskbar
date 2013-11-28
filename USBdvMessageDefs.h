/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _USBDVMESSAGEDEFS_H
#define _USBDVMESSAGEDEFS_H
/******************************************/
/* definitions des message est constantes */
/******************************************/

// preference
#define		USB_REFRESH_PREFERENCES_MSG		'Urfp'

// mount/unmount device
#define		USB_MOUNT_DEVICE_MSG			'Umdm'
#define		USB_UNMOUNT_DEVICE_MSG			'Uudm'
#define		USB_DEVICE_POINTER				"usb-device-pointer"

// fenetre d'info
#define		USB_WINDOW_INFO_MSG				'Uwim'
#define		USB_WINDOW_POSITION				"usb-window-pos"

// sons
#define		SYSTEM_SOUND_MOUNT_USB_DEVICE		"USB device mount"
#define		SYSTEM_SOUND_UNMOUNT_USB_DEVICE		"USB device unmount"

#endif
