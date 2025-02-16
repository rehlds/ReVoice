#pragma once

#include "appversion.h"

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <stddef.h>
#else //WIN32
	#include <arpa/inet.h>
	#include <dlfcn.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <sys/sysinfo.h>
	#include <sys/time.h>
	#define HIDDEN __attribute__((visibility("hidden")))
#endif //WIN32
