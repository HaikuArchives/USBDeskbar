/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/*************************/
/* vue pour le replicant */
/*************************/
#include "USBDeskbarView.h"
#include "USBDeskbarRoster.h"
#include "USBdvMessageDefs.h"
#include "USBPreferenceFile.h"
#include "USBDeskbarInfoWin.h"
#include <app/Message.h>
#include <interface/InterfaceDefs.h>
#include <Deskbar.h>
#include <support/Beep.h>
#include <interface/Bitmap.h>
#include <storage/File.h>
#include <app/Roster.h>
#include <support/DataIO.h>
#include <interface/PopUpMenu.h>
#include <interface/Menu.h>
#include <interface/MenuItem.h>
#include <support/String.h>

#include <Path.h>
#include <Alert.h>

// fonction externe pour instancier un replicant
extern "C" _EXPORT BView* instantiate_deskbar_item();

/**** creation de l'instance ****/
BView* instantiate_deskbar_item(void)
{	return new USBDeskbarView();	}

/**** constructeur ****/
USBDeskbarView::USBDeskbarView()
: BView(BRect(0,0,15,15),"USBDeskbarView", B_FOLLOW_NONE, B_WILL_DRAW | B_PULSE_NEEDED)
{
}

/**** constructeur a partir d'un BMessage ****/
USBDeskbarView::USBDeskbarView(BMessage *message)
: BView(message)
{
}

/**** destructeur ****/
USBDeskbarView::~USBDeskbarView()
{
}

/**** instanciate ****/
USBDeskbarView *USBDeskbarView::Instantiate(BMessage *data)
{
	if (!validate_instantiation(data,"USBDeskbarView"))
		return NULL;
	
	return (new USBDeskbarView(data));
}

/**** archivage de la vue ****/
status_t USBDeskbarView::Archive(BMessage *data, bool deep) const
{
	BView::Archive(data, deep);
	data->AddString("add_on","application/x-vnd.CKJ.USBDeskbarView");

	return B_NO_ERROR;
}

/**** gestion des messages ****/
void USBDeskbarView::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
	// on ajoute un periph USB
	case USB_MOUNT_DEVICE_MSG:
		MountDevice(message);
		break;
	// on enleve un periph USB
	case USB_UNMOUNT_DEVICE_MSG:
		UnmountDevice(message);
		break;
	// on quitte le fenetre d'info
	case USB_WINDOW_INFO_MSG:
		{
			BRect	frame;
			
			// recuperer la position
			if(message->FindRect(USB_WINDOW_POSITION,&frame)==B_OK)
				_prefs->SetRect(frame,USB_WINDOW_POSITION);
			
			// reinitialiser	
			_winInfo = NULL;
			_indexSee = -1;
		}
		break;
	default:
		BView::MessageReceived(message);
	}
}

// =============
// = Affichage =
// =============

/**** dessin ****/
void USBDeskbarView::Draw(BRect updateRect)
{
	// color of the deskbar tray view
	SetDrawingMode(B_OP_COPY);
	SetHighColor(Parent()->ViewColor());
	FillRect(Bounds());
	
	SetDrawingMode(B_OP_OVER);
	// display bitmap
	if(_icon!=NULL)
		DrawBitmap(_icon,BPoint(0,0));
	
	// dessin du mount ou unmount
	if(_mountDraw)
		DrawBitmap(_mountIcon,BPoint(0,0));
	
	if(_unmountDraw)
		DrawBitmap(_unmountIcon,BPoint(0,0));

	SetDrawingMode(B_OP_COPY);
}

/**** animation ****/
void USBDeskbarView::Pulse()
{
	// verifier si on doit changer l'image
	// on laisse affiché que 10 seconde
	if((_mountDraw || _unmountDraw) && (time(0) > _startAnimate + 10))
	{
		_mountDraw = false;
		_unmountDraw = false;
		Invalidate();	
	}
}

/**** clique souris ****/
void USBDeskbarView::MouseDown(BPoint where)
{
	// display popupmenu
	BMenuItem 	*selected = _menu->Go(ConvertToScreen(where));
	int32		index = _menu->IndexOf(selected);
	if(index==(_menu->CountItems()-1))
	{
		// remove replicant
		BDeskbar	desk;
		desk.RemoveItem("USBDeskbarView");
	}
	else
	{
		BUSBDevice			*dev;
		
		// recuperer le device
		_indexSee = index;
		dev = (BUSBDevice *)(_deviceList.ItemAt(_indexSee));
		if(dev==NULL)
			return;
			
		// afficher la fenetre
		if(_winInfo==NULL)
			_winInfo = new USBDeskbarInfoWin(_prefs->GetRect(BRect(100,100,480,450),USB_WINDOW_POSITION),this);
		
		// definir les infos et afficher
		_winInfo->SetInfo(dev,_iconLarge);
		_winInfo->Show();
	}
}

// ==============================================
// = creation / destruction de la vue replicant =
// ==============================================

/**** attachement a la fenetre ****/
void USBDeskbarView::AttachedToWindow()
{
	BDeskbar			desk;
	entry_ref			ref;
	BFile				file;
	BRoster				roster;

	// recuperer le chemin de l'executable
	// pour extraire les images
	roster.FindApp("application/x-vnd.CKJ.USBDeskbarView",&ref);
	file.SetTo(&ref, B_READ_ONLY);
	_resource.SetTo(&file);

	// initialisation
	_usbRoster = NULL;
	_icon = NULL;
	_menu = NULL;
	_prefs = NULL;
	_winInfo = NULL;
	_mountDraw = false;
	_unmountDraw = false;
	_startAnimate = 0;
	_indexSee = -1;

	// ajouter le son au systeme
	add_system_beep_event(SYSTEM_SOUND_MOUNT_USB_DEVICE);
	add_system_beep_event(SYSTEM_SOUND_UNMOUNT_USB_DEVICE);

	// creer le roster
	// et l'image
	_usbRoster = new USBDeskbarRoster(this);
	_icon = GetBitmap("USBDeskBarIcon");
	_mountIcon = GetBitmap("USBMount");
	_unmountIcon = GetBitmap("USBUnmount");
	_menu = new BPopUpMenu("devices-menu");
	_prefs = new USBPreferenceFile("USBDeskbarView");
	_iconLarge = GetBitmap("USBLargeIcon");
		
	// attachement
	BView::AttachedToWindow();

	// charger les prefs
	_prefs->Load();

	// recreer le menu
	BuildMenu();

	// demarrer le roster usb
	_usbRoster->Start();
}

/**** detachement ****/
void USBDeskbarView::DetachedFromWindow()
{
	// sauver les prefs avant de quitter
	_prefs->Save();

	// si la fenetre est ouverte la fermer
	if(_winInfo!=NULL)
		_winInfo->PostMessage(B_QUIT_REQUESTED);

	// stopper le roster
	_usbRoster->Stop();

	// liberer la memoire
	delete _usbRoster;
	delete _icon;
	delete _mountIcon;
	delete _unmountIcon;
	delete _iconLarge;
	delete _menu;
	delete _prefs;

	// detacher
	BView::DetachedFromWindow();
}

// =================
// = Creation Menu =
// =================

/**** creation du menu en fonction de la liste d'item ****/
void USBDeskbarView::BuildMenu()
{
	BUSBDevice	*device = NULL;
	BString		product;
	
	// on va parcourir la liste et ajouter les item
	delete _menu;
	
	// recreer
	_menu = new BPopUpMenu("devices-menu");
	for(int32 index=0;index<_deviceList.CountItems();index++)
	{
		device = (BUSBDevice *)(_deviceList.ItemAt(index));
		if(device!=NULL)
		{
			// on affiche ce que c'est et qui la fabriqué
			product = device->ProductString();
			product << " : " << device->ManufacturerString();
			
			// ajouter l'item
			_menu->AddItem(new BMenuItem(product.String(),NULL));
		}
	}	
	
	// le separateur
	if(_menu->CountItems()>0)
		_menu->AddSeparatorItem();
	
	// sortir
	_menu->AddItem(new BMenuItem("Exit",NULL));
}

// =================================
// = ajout/retrait de peripherique =
// =================================

/**** ajout periph USB ****/
void USBDeskbarView::MountDevice(BMessage *device)
{
	BUSBDevice	*dev = NULL;
	
	// recuperer le pointer sur le device
	if(device->FindPointer(USB_DEVICE_POINTER,(void **)&dev)!=B_OK)
		return;

	// ajouter l'item dans la liste
	_deviceList.AddItem(dev);

	// recreer le menu
	BuildMenu();

	// signal sonore
	system_beep(SYSTEM_SOUND_MOUNT_USB_DEVICE);
	
	// dessiner l'icon
	_startAnimate = time(0);
	_mountDraw = true;
	Invalidate();
}

/**** retirer un periph USB ****/
void USBDeskbarView::UnmountDevice(BMessage *device)
{
	BUSBDevice	*dev = NULL;
	
	// recuperer le pointer sur le device
	if(device->FindPointer(USB_DEVICE_POINTER,(void **)&dev)!=B_OK)
		return;

	// verifier si c'est la fenetre que l'on affiche
	if(_deviceList.IndexOf(dev)==_indexSee)
		_winInfo->PostMessage(B_QUIT_REQUESTED);

	// enlever l'item de la liste
	_deviceList.RemoveItem(dev);

	// recreer le menu
	BuildMenu();

	// signal sonore
	system_beep(SYSTEM_SOUND_UNMOUNT_USB_DEVICE);

	// dessiner l'icon
	_startAnimate = time(0);
	_unmountDraw = true;
	Invalidate();
}

// =================
// = gestion image =
// =================

/**** trouver une image dans les ressources ****/
BBitmap *USBDeskbarView::GetBitmap(const char *name)
{
	BBitmap 	*bitmap = NULL;
	size_t 		len = 0;
	status_t 	error;	

	// charger depuis les ressources
	const void *data = _resource.LoadResource('BBMP', name, &len);

	BMemoryIO stream(data, len);
	
	// charge l'image archivé
	BMessage archive;
	error = archive.Unflatten(&stream);
	if (error != B_OK)
		return NULL;

	// on va essayer de la recreer
	bitmap = new BBitmap(&archive);
	if(!bitmap)
		return NULL;

	// verifier que ca a marché
	if(bitmap->InitCheck() != B_OK)
	{
		delete bitmap;
		return NULL;
	}
	
	// tout c'est bien passé
	return bitmap;
}
