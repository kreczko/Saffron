/*
 * SafRawDataSet.h
 *
 *  Created on: Dec 13, 2014
 *      Author: Daniel Saunders
 *        Desc: Raw data container. Implemented in a data oriented way,
 *              as opposed to object oriented, for performance reasons.
 *              Created by the initlizer of the event builder.
 */


#ifndef SAFRAWDATASET_H_
#define SAFRAWDATASET_H_
#include "SafRunner.h"
#include "SafDataSet.h"
#include "SafRawDataChannel.h"


// Forward declarations.
class SafRunner;
class SafRawDataChannel;


class SafRawDataSet : SafDataSet
{
private:
	// Channel holder. Indexed by glib, then channel.
	std::vector< std::vector < SafRawDataChannel* > > m_channels;


public:
	SafRawDataSet(SafRunner * runner);
	virtual ~SafRawDataSet();

	int ithTime(unsigned int iglib, unsigned int ichan, unsigned int i);
	double ithSignal(unsigned int iglib, unsigned int ichan, unsigned int i);
	int ithTriggerTime(unsigned int iglib, unsigned int ichan, unsigned int i);
	double ithTriggerValue(unsigned int iglib, unsigned int ichan, unsigned int i);
	unsigned int ithNEntries(unsigned int iglib, unsigned int ichan);
	unsigned int ithNTriggers(unsigned int iglib, unsigned int ichan);
	void clear();
	SafRawDataChannel * channel(unsigned int iglib, unsigned int ichan);
};

#endif /* SAFRAWDATASET_H_ */
