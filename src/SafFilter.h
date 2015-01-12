/*
 * SafFilter.h
 *
 *  Created on: Dec 22, 2014
 *      Author: Daniel Saunders
 */

#ifndef SAFFILTER_H_
#define SAFFILTER_H_

#include "SafAlgorithm.h"
#include "SafRunner.h"
#include "SafRawDataChannel.h"


class SafRawDataChannel;

class SafFilter: public SafAlgorithm
{
private:
	// Members __________________________________________________________________
  unsigned int m_filterSize;

public:
  // Methods __________________________________________________________________
  SafFilter(SafRunner * runner);
  virtual ~SafFilter();

  void initialize();
  void execute();
  void finalize();
  void threadExecute(unsigned int iGlib, unsigned int iLow, unsigned int iUp,
    int iThread);
  void filterChannel(SafRawDataChannel * channel);
};

#endif /* SAFFILTER_H_ */
