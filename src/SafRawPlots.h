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
	std::vector<TH1F*> h_firstEventWaveforms;
	std::vector<TH1F*> h_signals;
	TH2F* h_allSignals;
	TH1F* h_signalMeans;
	TH1F* h_signalWidths;
	bool m_threading;


public:
	// Methods __________________________________________________________________
  SafRawPlots(SafRunner * runner);
	virtual ~SafRawPlots();

	void initialize();
	void execute();
	void finalize();

	void fill(int threadID);
};

#endif /* SAFRAWPLOTS_H_ */
