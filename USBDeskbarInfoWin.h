/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _USBDESKBARINFOWIN_H
#define _USBDESKBARINFOWIN_H
/***********************/
/* fenetre pour l'info */
/***********************/
#include <interface/Window.h>
#include <interface/View.h>
#include <support/String.h>

class USBDevice;
class BHandler;
class BMessenger;
class BTextView;
class BitmapView;
class USBConfiguration;
class USBInterface;
class USBEndpoint;

class USBDeskbarInfoWin : public BWindow
{
public:
	USBDeskbarInfoWin(BRect frame,BHandler *view);
	virtual	~USBDeskbarInfoWin();

	virtual	bool	QuitRequested();						// quitter la fenetre
			void	SetInfo(USBDevice *dev,BBitmap *icon);	// definir les infos a afficher

protected:
	USBDevice	*_device;			// device d'ou prendre les infos
	BMessenger	*_viewMessenger;	// pour envoyer des message a la vue
	BTextView	*_initialInfo;		// info standard
	BTextView	*_specialsInfo;		// info speciales	
	BitmapView	*_bitmapView;		// pour l'icon

			void	AddConfiguration(BString &buffer,const USBConfiguration *config);			// lister les configurations
			void	AddInterface(BString &buffer,const USBInterface *interface);				// lister les interfaces
			void	AddEndPoint(BString &buffer,const USBEndpoint *endpoint);					// lister les EndPoint
			void	AddClassName(BString &buffer,uint8 Class);									// ajouter le nom de la classe
			void	AddSubclassName(BString &buffer,uint8 Class,uint8 Subclass);				// type de la sous-classe
			void	AddProtocolName(BString &buffer,uint8 Class,uint8 Subclass,uint8 Protocol);	// nom du protocole
};

// classe juste pour afficher l'icon
class BitmapView : public BView
{
public:
	BitmapView();
	
	virtual	void		Draw(BRect updateRect);						// dessin
	inline	void		SetIcon(BBitmap *icon)	{ _icon = icon; }	// definir l'icon

protected:
	BBitmap		*_icon;		// icon
};

#endif
