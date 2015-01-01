/*
 * SafDataSet.cpp
 *
 *  Created on: Dec 13, 2014
 *      Author: ds9133
 */

#include "SafDataSet.h"


//_____________________________________________________________________________

SafDataSet::SafDataSet(SafRunner * runner, std::string name)
{
	m_runner = runner;
	m_childClassName = name;
}


//_____________________________________________________________________________

SafDataSet::~SafDataSet()
{
	// TODO Auto-generated destructor stub
}


//_____________________________________________________________________________

