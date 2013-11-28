/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _USBDESKBARROSTER_H
#define _USBDESKBARROSTER_H
/**************/
/* USB Roster */
/**************/
#include "USBKit.h"

class BMessenger;
class BHandler;

class USBDeskbarRoster : public USBRoster
{
public:
	USBDeskbarRoster(BHandler *view);
	~USBDeskbarRoster();
	
	virtual	status_t	DeviceAdded(USBDevice *dev);		// ajout d'un periph
	virtual	void		DeviceRemoved(USBDevice *dev);			// enlever un periph

protected:
	BMessenger		*_viewMessenger;	// messenger pour envoyer a la vue les infos
};

#endif
