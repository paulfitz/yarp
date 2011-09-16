/*
 *  Yarp Modules Manager
 *  Copyright: 2011 (C) Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 * 
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include <iostream>
#include <gtkmm/main.h>

#include <yarp/os/Network.h>
#include <yarp/os/Property.h>
#include <yarp/os/ConstString.h>

#define UNIX

#ifdef UNIX
	#include <errno.h>
	#include <sys/types.h>
	#include <signal.h>
#endif


#include "main_window.h"

using namespace std;

#define HELP_MESSAGE		"\
Usage:\n\
      gymanager [option...]\n\n\
Options:\n\
  --help                  Show help\n\
  --config                Configuration file name\n"

#define DEF_CONFIG_FILE		"./ymanager.ini"


void onSignal(int signum)
{
	cout<<"Use <quit> menu to exit!"<<endl;
}


int main(int argc, char *argv[])
{

	yarp::os::Network yarp;
	yarp.setVerbosity(-1);

	yarp::os::Property cmdline;
	yarp::os::Property config;
	
	cmdline.fromCommand(argc, argv);
	
	if(cmdline.check("help"))
	{
		cout<<HELP_MESSAGE<<endl;
		return 0;
	}
		
	if(cmdline.check("config"))
	{
		if(cmdline.find("config").asString() == "")
		{
			cout<<HELP_MESSAGE<<endl;
			return 0;			
		}
		if(!config.fromConfigFile(cmdline.find("config").asString().c_str()))
			cout<<"WARNING: "<<cmdline.find("config").asString().c_str()<<" cannot be loaded."<<endl;
	}
	else 
		config.fromConfigFile(DEF_CONFIG_FILE);

	/**
	 *  preparing default options
	 */
	if(!config.check("apppath"))
		config.put("apppath", "./");

	if(!config.check("modpath"))
		config.put("modpath", "./");
	
	if(!config.check("load_subfolders"))
		config.put("load_subfolders", "no");
	
	if(!config.check("watchdog"))
		config.put("watchdog", "no");

	if(!config.check("module_failure"))
		config.put("module_failure", "prompt");
	
	if(!config.check("connection_failure"))
		config.put("connection_failure", "prompt");

	if(!config.check("auto_connect"))
		config.put("auto_connect", "no");
	
	if(!config.check("auto_dependency"))
		config.put("auto_dependency", "no");

#ifdef UNIX
	struct sigaction new_action, old_action;
     
	/* Set up the structure to specify the new action. */
	new_action.sa_handler = onSignal;
	sigemptyset (&new_action.sa_mask);
	new_action.sa_flags = 0;

	sigaction (SIGINT, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
		sigaction (SIGINT, &new_action, NULL);
	sigaction (SIGHUP, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
		sigaction (SIGHUP, &new_action, NULL);
	sigaction (SIGTERM, NULL, &old_action);
	if (old_action.sa_handler != SIG_IGN)
		sigaction (SIGTERM, &new_action, NULL);

//	signal(SIGTERM, YConsoleManager::onExit());
//    signal(SIGINT, YConsoleManager::onExit());
#endif


	Gtk::Main kit(argc, argv);
	MainWindow window(config);

	//Shows the window and returns when it is closed.
	Gtk::Main::run(window);
	return 0;
}

