/*
 * SafRawDataChannel.cpp
 *
 *  Created on: Dec 13, 2014
 *      Author: Daniel Saunders
 */

#include "SafRawDataChannel.h"


//_____________________________________________________________________________

SafRawDataChannel::SafRawDataChannel(unsigned int glibID, unsigned int channelID,
	SafRawDataSet * rawData, SafRunner * runner) :
	SafDataSet(runner, "SafRawDataChannel"),
	m_nEntries(0),
	m_nTriggers(0),
	m_nEntriesTotal(0),
	m_nTriggersTotal(0)
{
  m_glibID = glibID;
  m_channelID = channelID;
}


//_____________________________________________________________________________

SafRawDataChannel::~SafRawDataChannel()
{
	// TODO Auto-generated destructor stub
}


//_____________________________________________________________________________

void SafRawDataChannel::clear()
{
	times()->clear();
	signals()->clear();

	times()->reserve(runner()->eventTimeWindow());
	signals()->reserve(runner()->eventTimeWindow());

	m_nEntries = 0;
	m_nTriggers = 0;
}


//_____________________________________________________________________________

unsigned int SafRawDataChannel::plotIndex() {
	return runner()->geometry()->nChannels() * m_glibID + m_channelID;
}


//_____________________________________________________________________________
