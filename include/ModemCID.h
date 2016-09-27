/*
    ModemCID - Fax Modem CallerID Detection
    Copyright (C) 2010-2014 MZSW Creative Software

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    MZSW Creative Software
    contato@mzsw.com.br
*/

/** @file ModemCID.h
 *  Main include header for the ModemCID library
 */

#ifndef _MODEMCID_H_
#define _MODEMCID_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MODEM_EVENT_CONNECTED 1
#define MODEM_EVENT_DISCONNECTED 2
#define MODEM_EVENT_RING 3
#define MODEM_EVENT_CALLERID 4
#define MODEM_EVENT_ONHOOK 5
#define MODEM_EVENT_OFFHOOK 6
#define MODEM_EVENT_HANGUP 7

typedef struct ModemEvent
{
	int type;
	char data[28];
} ModemEvent;

typedef struct ModemCID ModemCID;

#ifdef BUILD_DLL
# define LIBEXPORT __declspec(dllexport)
#else
#  ifdef LIB_STATIC
#    define LIBEXPORT
#  else
#    define LIBEXPORT extern
#  endif
#endif
#define LIBCALL __stdcall

#include "private/Platform.h"

/**
 * Start the connection using some configurations
 * 
 * parameters
 *   config: configuração da porta
 * 
 * return
 *   a pointer for a ModemCID instance
 */
LIBEXPORT ModemCID * LIBCALL ModemCID_create(const char* config);

/**
 * Check if it is connected to a modem
 * 
 * parameters
 *   lib: pointer for a ModemCID instance
 * 
 * return
 *   1 for connected, 0 otherwise
 */
LIBEXPORT int LIBCALL ModemCID_isConected(ModemCID * lib);

/**
 * Change connection configuration for modem
 * 
 * parameters
 *   lib: pointer for a ModemCID instance
 *   config: new configuration, including connection information
 */
LIBEXPORT void LIBCALL ModemCID_setConfiguration(ModemCID * lib, const char * config);

/**
 * Obtém a configuração atual da conexão com a balabça
 * 
 * parameters
 *   lib: pointer for a ModemCID instance
 * 
 * return
 *   a list of connection parameters splited with ;
 */
LIBEXPORT const char* LIBCALL ModemCID_getConfiguration(ModemCID * lib);

/**
 * Wait for a event
 * 
 * parameters
 *   lib: pointer for a ModemCID instance
 * 
 * return
 *   0 when instance is freed or canceled,
 *   otherwise a new event has picked from queue,
 */
LIBEXPORT int LIBCALL ModemCID_pollEvent(ModemCID * lib, ModemEvent* event);

/**
 * Cancel a poll event waiting call
 * 
 * parameters
 *   lib: pointer for a ModemCID instance
 */
LIBEXPORT void LIBCALL ModemCID_cancel(ModemCID * lib);

/**
 * Disconnect and free a instance
 * 
 * parameters
 *   lib: pointer for a ModemCID instance
 */
LIBEXPORT void LIBCALL ModemCID_free(ModemCID * lib);


/**
 * Get the library version
 * 
 * parameters
 *   lib: pointer for a ModemCID instance
 */
LIBEXPORT const char* LIBCALL ModemCID_getVersion();

#ifdef __cplusplus
}
#endif

#endif /* _MODEMCID_H_ */