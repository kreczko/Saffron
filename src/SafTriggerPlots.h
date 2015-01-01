/*
 * SafTriggerPlots.h
 *
 *  Created on: Dec 30, 2014
 *      Author: Daniel Saunders
 */

#ifndef SAFTRIGGERPLOTS_H_
#define SAFTRIGGERPLOTS_H_

#include "SafAlgorithm.h"
#include "SafRunner.h"
#include "SafRawDataChannel.h"

class SafTriggerPlots: public SafAlgorithm
{
private:
	// Members __________________________________________________________________

	std::vector<TH1F*> h_firstEventPeaks;
	std::vector<TH1F*> h_triggerValues;

public:
  // Methods __________________________________________________________________
	SafTriggerPlots(SafRunner * runner);
	virtual ~SafTriggerPlots();

	void initialize();
	void execute();
	void finalize();

	void fill();
};

#endif /* SAFTRIGGERPLOTS_H_ */
