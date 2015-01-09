/*
 * SafEventBuilder.h
 *
 *  Created on: Dec 13, 2014
 *      Author: Daniel Saunders
 */

#ifndef SAFEVENTBUILDER_H_
#define SAFEVENTBUILDER_H_
#include "SafAlgorithm.h"
#include "SafRunner.h"
#include "TRandom3.h"
#include "TTree.h"
#include "TFile.h"


class SafEventBuilder: public SafAlgorithm
{
private:
	// Members __________________________________________________________________
	TRandom3 * m_randGen;
	double m_mean; //MC mode.
	double m_rms; //MC mode.
	unsigned int m_treePos;
	TTree * m_tree;
  int m_glib, m_glibchan, m_trigger, m_layer, m_chanx, m_chany;
	std::vector<int> * m_waveform;
	bool m_firstTime;


public:
	// Methods __________________________________________________________________
	SafEventBuilder(SafRunner * runner);
	virtual ~SafEventBuilder();

	void initialize();
	void execute();
	void finalize();

	void monteCarlo();
	void realData(unsigned int channelIndexUpper);
	TTree * tree() {return m_tree;}
	unsigned int treePos() {return m_treePos;}
};

#endif /* SAFEVENTBUILDER_H_ */
