/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
/***********************/
/* fenetre pour l'info */
/***********************/
#include "USBDeskbarInfoWin.h"
#include "USBdvMessageDefs.h"
#include <app/Messenger.h>
#include <interface/TextView.h>
#include <interface/Bitmap.h>
#include <interface/InterfaceDefs.h>
#include <interface/ScrollView.h>
#include <USBKit.h>
#include <stdio.h>

/**** constructeur ****/
USBDeskbarInfoWin::USBDeskbarInfoWin(BRect frame,BHandler *view)
: BWindow(frame,"No Title",B_TITLED_WINDOW,B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	BRect			bounds = Bounds();
	BView			*supportView = NULL;
	BScrollView		*supportScroll = NULL;
	
	// initialiser
	_device = NULL;
	_viewMessenger = NULL;
	
	// ajouter les vues
	supportView = new BView(bounds,"support-view",B_FOLLOW_ALL, B_WILL_DRAW);
	supportView->SetViewColor(tint_color(ui_color(B_MENU_BACKGROUND_COLOR),B_DARKEN_1_TINT));
	_bitmapView = new BitmapView();
	_initialInfo = new BTextView(BRect(40,5,bounds.right-5,bounds.top+135),"initial-info",BRect(0,0,bounds.right-45,bounds.top+130),new BFont(be_fixed_font),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	_initialInfo->SetViewColor(tint_color(ui_color(B_MENU_BACKGROUND_COLOR),B_DARKEN_1_TINT));
	_initialInfo->MakeEditable(false);
	_specialsInfo = new BTextView(BRect(5,140,bounds.right-20,bounds.top+345),"specials-info",BRect(0,0,bounds.right-20,bounds.top+205),new BFont(be_fixed_font),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	_specialsInfo->SetViewColor(tint_color(ui_color(B_MENU_BACKGROUND_COLOR),B_NO_TINT));
	_specialsInfo->MakeEditable(false);
	supportScroll = new BScrollView("support-scroll",_specialsInfo,B_FOLLOW_ALL_SIDES,B_WILL_DRAW,false,true);
	supportScroll->SetViewColor(tint_color(ui_color(B_MENU_BACKGROUND_COLOR),B_NO_TINT));
	
	// creer le messenger
	_viewMessenger = new BMessenger(view);

	// ajouter les vues
	AddChild(supportView);
	supportView->AddChild(_bitmapView);
	supportView->AddChild(_initialInfo);
	supportView->AddChild(supportScroll);
}

/**** destructeur ****/
USBDeskbarInfoWin::~USBDeskbarInfoWin()
{
	// liberer la memoire
	delete _viewMessenger;
}

/**** quitter la fenetre ****/
bool USBDeskbarInfoWin::QuitRequested()
{
	// informer la vue que l'on quitte
	BMessage	message(USB_WINDOW_INFO_MSG);
	message.AddRect(USB_WINDOW_POSITION,Frame());
	_viewMessenger->SendMessage(&message);

	// on quitte
	return true;
}

/**** definir les infos a afficher ****/
void USBDeskbarInfoWin::SetInfo(BUSBDevice *dev,BBitmap *icon)
{
	BString		buffer;
	
	Lock();
	// definir le device
	_device = dev;
	_bitmapView->SetIcon(icon);

	// ajouter les infos
	SetTitle(_device->ManufacturerString());
	buffer = "Product : ";
	buffer << _device->ProductString() << "\n";
	buffer << "Manufacturer : " << _device->ManufacturerString() << "\n";
	buffer << "SerialNumber : " << _device->SerialNumberString() << "\n";
	buffer << "Class : " << (int32)_device->Class() << "\n";
	buffer << "Subclass : " << (int32)_device->Subclass() << "\n";
	buffer << "Protocol : " << (int32)_device->Protocol() << "\n";
	buffer << "VendorID : " << (int32)_device->VendorID() << "\n";
	buffer << "ProductID : " << (int32)_device->ProductID() << "\n";
	buffer << "Version : " << (int32)_device->Version();

	// mettre a jour
	_initialInfo->SetText(buffer.String());

	// lister les configurations
	buffer = "";
	for(uint32 index=0;index<_device->CountConfigurations();index++)
	{
		buffer << "+ Configuration : " << index << "\n";
		AddConfiguration(buffer,_device->ConfigurationAt(index));
	}
	
	// mettre a jour
	_specialsInfo->SetText(buffer.String());
	Unlock();
}

// =====================================
// = lister les point d'entree etc ... =
// =====================================

/**** lister les configurations ****/
void USBDeskbarInfoWin::AddConfiguration(BString &buffer,const BUSBConfiguration *config)
{
	// est t-elle valide
	if(config==NULL)
		return;

	// lister les interfaces
	for(uint32 index=0;index<config->CountInterfaces();index++)
	{
		buffer << "  + Interface : " << index << "\n";
		AddInterface(buffer,config->InterfaceAt(index));
	}	
}

/**** lister les interfaces ****/
void USBDeskbarInfoWin::AddInterface(BString &buffer,const BUSBInterface *interface)
{
	// est t-elle valide
	if(interface==NULL)
		return;
	
	// infos sur l'interface
	buffer << "    Class : " << (uint32)interface->Class() << " (";
	AddClassName(buffer,interface->Class());
	buffer << ")\n";
	
	buffer << "    SubClass : " << (uint32)interface->Subclass() << " (";
	AddSubclassName(buffer,interface->Class(),interface->Subclass());
	buffer << ")\n";
	
	buffer << "    Protocol : " << (uint32)interface->Protocol() << " (";
	AddProtocolName(buffer,interface->Class(),interface->Subclass(),interface->Protocol());
	buffer << ")\n";
	
	// ajouter les endpoints
	for(uint32 index=0;index<interface->CountEndpoints();index++)
	{
		buffer << "    + EndPoint : " << index << "\n";
		AddEndPoint(buffer,interface->EndpointAt(index));
	}	
	
	// ajouter les descripteurs
	uchar			data[256];
	usb_descriptor	*description = (usb_descriptor *)data;
	char			hexByteStr[2];
	
	for(uint32 index=0;interface->OtherDescriptorAt(index,description,256)==B_OK;index++)
	{
		buffer << "    + Descriptor : " << index << "\n";
		for(uint8 dIndex=0;description->generic.length;dIndex++)
		{
			sprintf(hexByteStr, "%02x", description->generic.data[dIndex]);
			buffer << "      generic value= 0x" << hexByteStr << "\n";
		}
	}
}

/**** lister les EndPoint ****/
void USBDeskbarInfoWin::AddEndPoint(BString &buffer,const BUSBEndpoint *endpoint)
{
	// est t-il valide
	if(endpoint==NULL)
		return;
	
	// ajouter les infos
	buffer << "      MaxPacketSize= " << (uint32)endpoint->MaxPacketSize() << "\n";
	buffer << "      Interval= " << (uint32)endpoint->Interval() << "\n";
	
	// type
	if (endpoint->IsBulk())
		buffer << "      Bulk\n";
	if (endpoint->IsIsochronous())
		buffer << "      Isochronous\n";
	if (endpoint->IsInterrupt())
		buffer << "      Interrupt\n";
	if (endpoint->IsInput())
		buffer << "      Direction= Input\n";
	else
		buffer << "      Direction= Output\n";
}

// ========================
// = definition des types =
// ========================

/**** ajouter le nom de la classe ****/
void USBDeskbarInfoWin::AddClassName(BString &buffer,uint8 Class)
{
	switch(Class)
	{
	case 1:
		buffer << "Audio";
		break;
	case 2:
		buffer << "Communication";
		break;
	case 3:
		buffer << "Human interface";
		break;
	case 5:
		buffer << "Physical";
		break;
	case 6:
		buffer << "Image";
		break;
	case 7:
		buffer << "Printer";
		break;
	case 8:
		buffer << "Mass storage";
		break;
	case 9:
		buffer << "Hub";
		break;
	case 10:
		buffer << "Data interface";
		break;
	case 11:
		buffer << "Smart card";
		break;
	case 13:
		buffer << "Content-security";
		break;
	case 255:
		buffer << "Vendor specific";
		break;
	default:
		buffer << "?";
	}
}

/**** type de la sous-classe ****/
void USBDeskbarInfoWin::AddSubclassName(BString &buffer,uint8 Class,uint8 Subclass)
{
	switch(Class)
	{
	// audio
	case 1:
		switch (Subclass)
		{
		// class 1 subclass 1
		case 1:
			buffer << "Audio control";
			break;
		case 2:
			buffer << "Audio streaming";
			break;
		case 3:
			buffer << "MIDI streaming";
			break;
		default:
			buffer << "?";
		}
		break;
	case 10:
		if (Subclass == 255)
			buffer << "Webcam";
		else
			buffer << "?";
		break;
	default:
		buffer << "?";
	}
}

/**** nom du protocole ****/
void USBDeskbarInfoWin::AddProtocolName(BString &buffer,uint8 Class,uint8 Subclass,uint8 Protocol)
{
	switch (Class)
	{
	case 3: // "Human interface";
		switch (Subclass)
		{
		case 1:
			switch (Protocol)
			{
			case 1:
				buffer << "Keyboard";					//	3_1_1
				break;
			case 2:
				buffer << "Mouse";						//	3_1_2
				break;
			case 3:
				buffer << "Tablet";					//	3_1_3
				break;
			default:
				buffer << "?";
			}
			break;
		default:
			buffer << "?";
		}
		break;
	case 7: // "Printer";
		switch (Subclass)
		{
		case 1:
			switch (Protocol)
			{
			case 1:
				buffer << "Unidirectional";			//	7_1_1
				break;
			case 2:
				buffer << "Bi-directional";			//	7_1_2
				break;
			case 3:
				buffer << "IEEE 1284.4 compatible";	//	7_1_3
				break;
			default:
				buffer << "?";
			}
			break;
		default:
			buffer << "?";
		}
		break;
	default:
		buffer << "?";
	}
}

// ===============================
// = classe pour afficher l'icon =
// ===============================

/**** constructeur ****/
BitmapView::BitmapView()
: BView(BRect(5,5,36,36),"support-view",B_FOLLOW_NONE, B_WILL_DRAW)
{
	// initialiser
	_icon = NULL;
}

/**** dessin ****/
void BitmapView::Draw(BRect updateRect)
{
	// color of the deskbar tray view
	SetHighColor(tint_color(ui_color(B_MENU_BACKGROUND_COLOR),B_DARKEN_1_TINT));
	FillRect(Bounds());
	
	// display bitmap
	if(_icon!=NULL)
	{
		SetDrawingMode(B_OP_OVER);
		DrawBitmap(_icon,BPoint(0,0));
		SetDrawingMode(B_OP_COPY);	
	}
}
