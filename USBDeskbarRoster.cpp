/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/**************/
/* USB Roster */
/**************/
#include "USBDeskbarRoster.h"
#include "USBdvMessageDefs.h"
#include <Messenger.h>

/**** constructeur ****/
USBDeskbarRoster::USBDeskbarRoster(BHandler *view)
{
	// initialiser le messenger
	_viewMessenger = new BMessenger(view);
}

/**** destructeur ****/
USBDeskbarRoster::~USBDeskbarRoster()
{
	// liberer la memoire
	delete _viewMessenger;
}

/**** Ajout d'un peripherique ****/	
status_t USBDeskbarRoster::DeviceAdded(BUSBDevice *dev)
{
	// verifier que le divice est bon
	if(dev==NULL)
		return B_OK;
	
	// on affiche pas le Hub
	if(dev->IsHub())
		return B_OK;
	
	// envoyer le message a la vue
	BMessage	mount(USB_MOUNT_DEVICE_MSG);
	
	// ajouter le pointer sur de device
	mount.AddPointer(USB_DEVICE_POINTER,dev);
	if(_viewMessenger->IsValid())
		_viewMessenger->SendMessage(&mount);
	
	// ok tout c'est bien passé
	return B_OK;
}

/**** Enlevement d'un peripherique ****/	
void USBDeskbarRoster::DeviceRemoved(BUSBDevice *dev)
{
	// envoyer le message a la vue
	BMessage	unmount(USB_UNMOUNT_DEVICE_MSG);
	
	// ajouter le pointer sur de device
	unmount.AddPointer(USB_DEVICE_POINTER,dev);
	if(_viewMessenger->IsValid())
		_viewMessenger->SendMessage(&unmount);
}
