/*
 * SafDataSet.h
 *
 *  Created on: Dec 13, 2014
 *      Author: ds9133
 *        Desc: Base class for all data sets in Saffron.
 */

#ifndef SAFDATASET_H_
#define SAFDATASET_H_
#include <iostream>


// Forward declarations.
class SafRunner;


class SafDataSet
{
private:
	SafRunner * m_runner;
	std::string m_childClassName;


public:
	// Methods __________________________________________________________________
	SafDataSet(SafRunner * runner, std::string);
	virtual ~SafDataSet();

	SafRunner * runner() {return m_runner;}
};

#endif /* SAFDATASET_H_ */
