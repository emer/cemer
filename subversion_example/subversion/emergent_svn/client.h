/*
 * client.h
 *
 *  Created on: Nov 9, 2012
 *      Author: houman
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "pool.h"

// Apr

// subversion
#include <svn_path.h>

// std
#include <iostream>
#include <vector>

// eventually have to implement our own context class
#include "../kdevsvncpp/context.hpp"

class Client {
public:
	Client();
	virtual ~Client();
	int Checkout(Pool pool, const char *url, const char *working_copy_path, int rev = -1);
	bool Update(Pool pool, const char * path, int rev = -1);
};

#endif /* CLIENT_H_ */
