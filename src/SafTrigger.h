/*
 * SafTrigger.h
 *
 *  Created on: Dec 22, 2014
 *      Author: Daniel Saunders
 */

#ifndef SAFTRIGGER_H_
#define SAFTRIGGER_H_

#include "SafAlgorithm.h"
#include "SafRunner.h"
#include "SafRawDataChannel.h"

class SafRawDataChannel;

class SafTrigger: public SafAlgorithm
{
private:
	// Members __________________________________________________________________
  unsigned int m_triggerWindowSizeTotal;
  unsigned int m_triggerWindowSizeA;
  unsigned int m_triggerWindowSizeB;
  unsigned int m_triggerWindowSizeC;
  double m_triggerValueCut;


public:
  // Methods __________________________________________________________________
	SafTrigger(SafRunner * runner);
	virtual ~SafTrigger();

	void initialize();
	void execute();
	void finalize();

	void scan(SafRawDataChannel * channel);
	double evalTimeWindow(std::vector<double> * signals,
		std::vector<int> * times, unsigned int i);
};

#endif /* SAFTRIGGER_H_ */
