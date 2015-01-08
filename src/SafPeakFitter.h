/*
 * SafPeakFitter.h
 *
 *  Created on: Dec 22, 2014
 *      Author: Daniel Saunders
 */

#ifndef SAFPEAKFITTER_H_
#define SAFPEAKFITTER_H_

#include "SafAlgorithm.h"
#include "SafRunner.h"
#include "SafRawDataChannel.h"


class SafRawDataChannel;

class SafPeakFitter: public SafAlgorithm
{
private:
	// Members __________________________________________________________________


public:
  // Methods __________________________________________________________________
	SafPeakFitter(SafRunner * runner);
	virtual ~SafPeakFitter();

	void initialize();
	void execute();
	void finalize();
  void threadExecute(unsigned int iGlib, unsigned int iLow, unsigned int iUp);
};

#endif /* SafPeakFitter_H_ */
