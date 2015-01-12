/*
 * SafRawPlots.h
 *
 *  Created on: Dec 15, 2014
 *      Author: ds9133
 */

#ifndef SAFRAWPLOTS_H_
#define SAFRAWPLOTS_H_

#include "SafAlgorithm.h"
#include "SafRunner.h"
#include <sstream>
#include "TH1F.h"
#include "TH2F.h"
#include "TF1.h"
#include "TDirectory.h"


class SafRawPlots: public SafAlgorithm
{
private:
	// Members __________________________________________________________________
	std::vector<TH1F*> * h_firstEventWaveforms;
	std::vector<TH1F*> * h_signals;
	TH2F * h_allSignals;
	TH1F * h_allSignalsProj;
	TH1F * h_signalMeans;
	TH1F * h_signalWidths;
	bool m_filtered;
	std::vector<TH2F*> h_avSignalPerEventPerChannelTemp;
	std::vector<TH2F*> h_rmsSignalPerEventPerChannelTemp;
	TH2F * h_avSignalPerEventPerChannel;
	TH2F * h_rmsSignalPerEventPerChannel;
	std::vector<double> m_avCache;
	std::vector<double> m_rmsCache;


public:
	// Methods __________________________________________________________________
  SafRawPlots(SafRunner * runner, bool filtered);
	virtual ~SafRawPlots();

	void initialize();
	void execute();
	void finalize();
  void threadExecute(unsigned int iGlib, unsigned int iLow, unsigned int iUp,
  	int iThread);
};

#endif /* SAFRAWPLOTS_H_ */
