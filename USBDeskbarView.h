/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#ifndef _USBDESKBARVIEW_H
#define _USBDESKBARVIEW_H
/*************************/
/* vue pour le replicant */
/*************************/
#include <interface/View.h>
#include <storage/Resources.h>
#include <support/List.h>
#include <time.h>

class BBitmap;
class USBDeskbarRoster;
class USBPreferenceFile;
class USBDeskbarInfoWin;
class BMessage;
class BPopUpMenu;

class USBDeskbarView : public BView
{
public:
	USBDeskbarView();
	USBDeskbarView(BMessage *message);
	virtual	~USBDeskbarView();

	static		USBDeskbarView		*Instantiate(BMessage *data);						// instanciate
	virtual		status_t 			Archive(BMessage *data, bool deep = true) const;	// archivage de la vue
	virtual		void				AttachedToWindow();
	virtual		void				DetachedFromWindow();

protected:
	BBitmap				*_icon;			// icon a afficher
	BBitmap				*_mountIcon;	// icon de mount
	BBitmap				*_unmountIcon;	// icon de unmount
	BBitmap				*_iconLarge;	// icon 32*32
	USBDeskbarRoster	*_usbRoster;	// surveillance USB
	USBPreferenceFile	*_prefs;		// preferences
	USBDeskbarInfoWin	*_winInfo;		// fenetre d'info
	BResources			_resource;		// fichier ressource
	bool				_mountDraw;		// afficher l'icon mount
	bool				_unmountDraw;	// afficher le unmount
	time_t				_startAnimate;	// demarrer l'animation
	BPopUpMenu			*_menu;			// menu pour voir les objets monté
	BList				_deviceList;	// liste des devices USB
	int32				_indexSee;		// index qui a ouvert une fenetre

	virtual	void		MessageReceived(BMessage *message);	// gestion des messages
	virtual	void		Draw(BRect updateRect);				// dessin
	virtual	void		Pulse();							// animation
	virtual	void		MouseDown(BPoint where);			// clique souris
			void		MountDevice(BMessage *device);		// ajout periph USB
			void		UnmountDevice(BMessage *device);	// retirer un periph USB
			void		BuildMenu();						// creation du menu en fonction de la liste d'item
			BBitmap		*GetBitmap(const char *name);		// trouver une image dans les ressources
};

#endif
