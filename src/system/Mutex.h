/*
    PesoLib - Biblioteca para obten��o do peso de itens de uma balan�a
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

/** @file Mutex.h
 *  mutex file header for the PesoLib library
 */

#ifndef _MUTEX_H_
#define _MUTEX_H_

typedef struct Mutex Mutex;

Mutex* Mutex_create();
void Mutex_lock(Mutex* mutex);
void Mutex_unlock(Mutex* mutex);
void Mutex_free(Mutex* mutex);

#endif /* _MUTEX_H_ */