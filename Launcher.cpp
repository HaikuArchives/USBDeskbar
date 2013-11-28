/***********************************************/
/* Software Created By CKJ Vincent Cedric      */
/* This code is the property of Vincent Cedric */
/* You can't copy or use this code without     */
/* according of the author (GPL License).	   */
/***********************************************/
#include "Launcher.h"
#include <Deskbar.h>
#include <File.h>
#include <Roster.h>
#include <Path.h>
#include <NodeInfo.h>
#include <stdio.h>

/**** Start ****/
int main(int argc, char *argv[])
{
	Launcher	*application;
	
	// start application in background
	application = new Launcher();
	delete application;

	return 0;
}

/**** Constructeur ****/
Launcher::Launcher(void)
: BApplication("application/x-vnd.CKJ.USBDeskbarView")
{
	BDeskbar			desk;

	// verifier qu'il n'est pas deja la
	if(desk.HasItem("USBDeskbarView")==false)
	{
		BRoster roster;
		entry_ref ref;

		// find application in roster
		if(roster.FindApp("application/x-vnd.CKJ.USBDeskbarView",&ref)!=B_OK)
			printf("Error Application not find\n");
		
		// add archived view from app
		if(desk.AddItem(&ref)!=B_OK)
			printf("Error on the Install of Replicant\n");
		
	}
}

/**** Destructeur ****/
Launcher::~Launcher()
{
}