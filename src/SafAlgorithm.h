/*
 * SafAlgorithm.h
 *
 *  Created on: Dec 13, 2014
 *      Author: Daniel Saunders
 *        Desc: Base class for all SafAlgorithms.
 */


#ifndef SAFALGORITHM_H_
#define SAFALGORITHM_H_
#include <iostream>
#include <vector>
#include "TH1F.h"
#include "TH2F.h"
#include <sstream>
#include <sys/time.h>
#include <mutex>
#include <thread>
#include <math.h>


// Forward declarations.
class SafRunner;


class SafAlgorithm
{
private:
	// Members __________________________________________________________________
	std::string m_childClassName; // Set by constructor of child.
	SafRunner * m_runner; // Pointer to the runner, set by constructor of child.
	bool m_eof; // Flag to show if read all data.
  std::vector< TH1F* > h_th1sSLOW; // Slow performance.
	std::vector< TH2F* > h_th2sSLOW; // Slow performance.
	unsigned long long m_totalTime;
	double m_avTime;
	std::mutex m_mtx;
	unsigned int m_nGlibs;
	unsigned int m_nChannels;


public:
	unsigned int m_nThreadsPerGlib;
	bool m_threading;
	unsigned int m_nChannelsPerThread;
	unsigned int m_event;
	bool m_forceSingleThread;

	// Methods __________________________________________________________________
	SafAlgorithm(SafRunner * runner, std::string name);
	virtual ~SafAlgorithm();

	virtual void initialize() {std::cout<<"Default initilizer."<<std::endl;}
	virtual void execute() {std::cout<<"Default execute."<<std::endl;}
	virtual void finalize() {std::cout<<"Default finalizer."<<std::endl;}
	virtual void threadExecute(unsigned int iGlib, unsigned int iChannelLow, 
		unsigned int iChannelUp, int iThread) 
	{std::cout<<"Default thread execute."<<std::endl;}

	void parentInitialize(unsigned int, unsigned int); // Should always use parent versions.
	void parentExecute();
	void parentFinalize();
	void parentThreadExecute();

	void plot(double x, std::string name, std::string title,
			unsigned int nBinsX, double xLow, double xHigh);
	void plot(double x, double weight, std::string name, std::string title,
			unsigned int nBinsX, double xLow, double xHigh);
	void plot(double x, double y, std::string name, std::string title,
			unsigned int nBinsX, double xLow, double xHigh, unsigned int nBinsY,
			double yLow, double yHigh);
	void plot(double x, double y, double weight, std::string name, std::string title,
		unsigned int nBinsX, double xLow, double xHigh, unsigned int nBinsY,
		double yLow, double yHigh);
	TH1F * findPlot(std::string name);
	std::vector<TH1F*> * initPerChannelPlots(std::string name, std::string title, 
		unsigned int nBins, double low, double up);


	// Setters and getters ______________________________________________________
	std::string name() {return m_childClassName;}
	bool eof() {return m_eof;}
	SafRunner * runner() {return m_runner;}
	unsigned int event() {return m_event;}
	double avTime() {return m_avTime;}
};

#endif /* SAFALGORITHM_H_ */
