/*
 * SafRawDataSet.cpp
 *
 *  Created on: Dec 13, 2014
 *      Author: Daniel Saunders
 */

#include "SafRawDataSet.h"

//_____________________________________________________________________________

SafRawDataSet::SafRawDataSet(SafRunner * tempRunner) :
	SafDataSet(tempRunner, "SafRawDataSet")
{
	runner()->setRawData(this);
	for (unsigned int ig = 0; ig < runner()->geometry()->nGlibs(); ig++) {
		std::vector< SafRawDataChannel* > glibChannels;
		for (unsigned int ic = 0; ic < runner()->geometry()->nChannels(); ic++)
			glibChannels.push_back(new SafRawDataChannel(ig, ic, this, runner()));

		m_channels.push_back(glibChannels);
	}
}


//_____________________________________________________________________________

SafRawDataSet::~SafRawDataSet()
{
	// TODO Auto-generated destructor stub
}


//_____________________________________________________________________________

void SafRawDataSet::clear()
{
	for (unsigned int ig = 0; ig < runner()->geometry()->nGlibs(); ig++) {
		for (unsigned int ic = 0; ic < runner()->geometry()->nChannels(); ic++)
			m_channels[ig][ic]->clear();
	}
}


//_____________________________________________________________________________

int SafRawDataSet::ithTime(unsigned int iglib, unsigned int ichan, unsigned int i)
{
	return channel(iglib, ichan)->times()->at(i);
}

double SafRawDataSet::ithSignal(unsigned int iglib, unsigned int ichan, unsigned int i)
{
		return channel(iglib, ichan)->signals()->at(i);
}

unsigned int SafRawDataSet::ithNEntries(unsigned int iglib, unsigned int ichan)
{
		return m_channels[iglib][ichan]->nEntries();
}

unsigned int SafRawDataSet::ithNTriggers(unsigned int iglib, unsigned int ichan)
{
		return m_channels[iglib][ichan]->nTriggers();
}

SafRawDataChannel * SafRawDataSet::channel(unsigned int iglib, unsigned int ichan)
{
	return m_channels[iglib][ichan];
}
