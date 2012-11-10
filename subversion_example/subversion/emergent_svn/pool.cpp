/*
 * Pool.cpp
 *
 *  Created on: Nov 9, 2012
 *      Author: houman
 */

#include "pool.h"

Pool::Pool() {
	apr_initialize();
	m_pool = svn_pool_create(0);
}

apr_pool_t * Pool::pool() {
	return m_pool;
}

Pool::~Pool() {
	svn_pool_destroy(m_pool);
	apr_terminate();
}

