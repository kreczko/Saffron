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


class SafRawDataChannel;


class SafEventBuilder: public SafAlgorithm
{
private:
	// Members __________________________________________________________________
	TRandom3 * m_randGen;
	double m_mean; //MC mode.
	double m_rms; //MC mode.
	std::vector<unsigned int> m_treePos;
	std::vector<TTree*> m_trees;
	std::vector<TFile*> m_files;
	std::vector<std::string> m_fileNames; // one per thread.
	std::vector<int> m_glibs;
	std::vector<int> m_glibchans;
	std::vector<int> m_triggers;
	std::vector<int> m_layers;
	std::vector<int> m_chanxs;
	std::vector<int> m_chanys;
	std::vector< std::vector<int>* > m_waveforms;
	std::vector<int> * m_spareWaveform;
	bool m_firstTime;
	unsigned int m_nFileThreads;



public:
	// Methods __________________________________________________________________
	SafEventBuilder(SafRunner * runner);
	virtual ~SafEventBuilder();

	void initialize();
	void execute();
	void finalize();

	void monteCarlo();
	void threadFill(SafRawDataChannel * channel, std::vector<int> * waveform);
	void realData(unsigned int channelIndexUpper);
	TTree * tree() {return m_trees.back();}
	unsigned int treePos() {return m_treePos.back();}
};

#endif /* SAFEVENTBUILDER_H_ */
