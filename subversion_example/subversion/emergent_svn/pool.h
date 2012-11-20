/*
 * Pool.h
 *
 *  Created on: Nov 9, 2012
 *      Author: houman
 */

#ifndef POOL_H_
#define POOL_H_

// apr
#include <apr_pools.h>

// svn
#include <svn_pools.h>

class Pool {
public:
  Pool();
  virtual ~Pool();
  apr_pool_t * pool();
private:
  apr_pool_t * m_pool;
};

#endif /* POOL_H_ */
