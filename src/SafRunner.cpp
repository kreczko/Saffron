/*
 * SafRunner.cpp
 *
 *  Created on: Dec 13, 2014
 *      Author: Daniel Saunders
 */

#include "SafRunner.h"

//_____________________________________________________________________________

SafRunner::SafRunner() :
  m_printThreshold(0),
  m_algorithms(),
  m_rawData(NULL),
  m_triggerData(NULL),
  m_eventTimeWindow(2048),
  m_event(0),
  m_timeZero(0),
  m_printRate(50),
  m_triggerSkip(0)
{
	// Default algorithm list.
	m_algorithms.push_back(new SafEventBuilder(this));
	//m_algorithms.push_back(new SafRawPlots(this, false));
  m_algorithms.push_back(new SafFilter(this));
  m_algorithms.push_back(new SafRawPlots(this, true));
//	m_algorithms.push_back(new SafTrigger(this));
//	m_algorithms.push_back(new SafTriggerPlots(this));
//	m_algorithms.push_back(new SafPeakFitter(this));
//	m_algorithms.push_back(new SafCoincidenceFinder(this));

	// Geometry.
	m_geometry = new SafGeometry();

	// Options.
	m_nEvents = 4400;
	m_runMode = 1; // 0 for MC, 1 for real data.

	// Save file.
	m_saveFile = new TFile("Saffron-histos-SM1_14Jan2015_2220_run0_scoperun_2.0V_500000event.root", "RECREATE");
}


//_____________________________________________________________________________

SafRunner::~SafRunner()
{
	m_saveFile->Close();
}


//_____________________________________________________________________________

void SafRunner::safPrint(std::string text, int level)
{
	std::cout<<text<<std::endl;
}


//_____________________________________________________________________________

void SafRunner::run()
{
	// Default runner of Saffron. In turn, calls all initialise, execute and
	// finalize methods of all SafAlgorithms used in this run.

	new SafRawDataSet(this);
	new SafTriggerDataSet(this);

	std::cout<<"Initializing all algorithms..."<<std::endl;
	std::vector< SafAlgorithm* >::iterator ialgo;
	for (ialgo = m_algorithms.begin(); ialgo != m_algorithms.end(); ialgo++)
		(*ialgo)->parentInitialize(geometry()->nGlibs(), geometry()->nChannels());

	struct timeval  tv1, tv2;
	gettimeofday(&tv1, NULL);

	std::cout<<"Executing event loop."<<std::endl;
	eventLoop();

	gettimeofday(&tv2, NULL);
	double totExTime = (unsigned long long) (tv2.tv_usec - tv1.tv_usec) 
			+ (unsigned long long) ((tv2.tv_sec - tv1.tv_sec)*1000000ULL);

	std::cout<<"Finalizing all algorithms..."<<std::endl;
	for (ialgo = m_algorithms.begin(); ialgo != m_algorithms.end(); ialgo++)
		(*ialgo)->parentFinalize();
	
	double totAvTime = 0;
	std::cout<<"\n--------- Algorithm Average Execute Time (us) ---------"<<std::endl;
	for (ialgo = m_algorithms.begin(); ialgo != m_algorithms.end(); ialgo++) {
		std::cout<<(*ialgo)->name()<<"\t\t"<<(*ialgo)->avTime()<<std::endl;
		totAvTime += (*ialgo)->avTime();
	}

	double scopeEquivRead = m_nEvents*2048*geometry()->nGlibs()*geometry()->nChannels()*8/1000000.;
	std::cout<<"\nTotal time sample processed (scope mode + skips): \t"<<2048*m_nEvents*16/1000000.<<" (ms)"<<std::endl;
	std::cout<<"Fraction tree read: \t\t\t\t"<<((SafEventBuilder*)m_algorithms[0])->treePos()/(1.*((SafEventBuilder*)m_algorithms[0])->tree()->GetEntries())<<std::endl;
	std::cout<<"Total algorithm average (per event): \t\t"<<totAvTime<<" (ms)"<<std::endl;
	std::cout<<"Total execution time (per event): \t\t"<<totExTime/(1000.*m_nEvents)<<" (ms)"<<std::endl;
	std::cout<<"Total execution time: \t\t\t\t"<<totExTime/1000000<<" (s)"<<std::endl;
}


//_____________________________________________________________________________

void SafRunner::eventLoop() {
  // Event loop.
	for (unsigned int i=0; i<m_nEvents; i++) {
		bool eof = false;
  	if (m_event % m_printRate == 0) {
  		double frac = ((SafEventBuilder*)m_algorithms[0])->treePos()/(1.*((SafEventBuilder*)m_algorithms[0])->tree()->GetEntries());
  		std::cout<<"Event: "<<m_event<<"\tFraction read: "<<frac<<std::endl;
  	}
		for (std::vector< SafAlgorithm* >::iterator ialgo = m_algorithms.begin(); 
			ialgo != m_algorithms.end(); ialgo++) {
		  (*ialgo)->parentExecute();
			if ((*ialgo)->eof()) eof = true;
	  }

		rawData()->clear();
		triggerData()->clear();

		if (eof) {
	  	std::cout<<"EOF"<<std::endl;
	  	break;
	  }
	  m_event++;
	}
}


//_____________________________________________________________________________
