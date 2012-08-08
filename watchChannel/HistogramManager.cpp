#include "HistogramManager.h"

#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <iostream>

#include "TPad.h"

using namespace std;

HistogramManager::HistogramManager():
	_hist("times", "", 1000, 0, 100),
	_interrupted(false),
	_thread(threadFunc, this)
{
	_hist.SetDirectory(0);
	_settings.host=string("127.0.0.1");
	_settings.port=1055;
	_settings.windowOffset = -2;
	_settings.windowWidth = 10;
	try {
		tryApply(_settings);
	} catch(...) {
	}
}

void * HistogramManager::threadFunc(void * arg) {
	HistogramManager * This = static_cast<HistogramManager*>(arg);
	assert(This);
	V1190BClient::Event event;
	cerr << "Readout thread started" << endl;
	while(true) {
		if(This->_interrupted)
			break;
		if (!This->_client.readEvent(event))
			break;
		typedef V1190BClient::Event::Measurements Measurements;
		float trigger = -1;
		if (This->_settings.subtractTrigger){
			for (Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++) {
				if (i->gChannel()!=0) continue;
				if (trigger >= 0) {
					trigger = -1;
					break;
				}
				trigger = 0.1 * i->time;
			}
			if (trigger < 0)
				continue;
		} else {
			trigger = 0;
		}

		vector<float> allTimes;
		for (Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++) {
			if (i->gChannel() != (int) This->_settings.channel) continue;
			allTimes.push_back(0.1 * i->time - trigger);
		}
		if (This->_settings.showOnlyFirstHit) 	{
			vector<float>::const_iterator i = min_element(allTimes.begin(), allTimes.end());
			if (i!=allTimes.end()) {
				This->_hist.Fill(*i);
			}
		} else {
			for (vector<float>::const_iterator i = allTimes.begin(); i!=allTimes.end(); ++i) {
				This->_hist.Fill(*i);
			}
		}
	}
	cerr << "Readout thread stopped" << endl;
	return 0;
}

void HistogramManager::apply(const Settings & settings) {
	try {
		tryApply(settings);
		_settings = settings;
	} catch (...) {
		try {
			tryApply(_settings);
		} catch (...) {
		}
		throw;
	}
}


static void join(TThread & thread) {
	TThread::EState state =  thread.GetState();
	switch(state){
	case TThread::kRunningState:
		thread.Join();
		break;
	}
}
void HistogramManager::tryApply(const Settings & settings)
{
	TThread::EState state =  _thread.GetState();
	_interrupted = true;
	_client.disconnect();
	join(_thread);

	string checkResult =settings.check();
	if (!checkResult.empty())
		throw runtime_error(checkResult);
	socketwrapper::Error rv = _client.connect(settings.host.c_str(), settings.port);
	if (rv != socketwrapper::OK) {
		throw runtime_error(socketwrapper::errorToString(rv));
	}
	TThread::Sleep(1);
	::apply(settings, _client);
	if (!_client)
		throw runtime_error("Connection failed");
	float binWidth = 0.1;
	float margin = 50;
	float histOffset = -margin;
	float histWidth = settings.windowWidth * 25 - histOffset + margin; // ns
	_hist.SetBins(int(histWidth/binWidth), histOffset, histOffset + histWidth);
	ostringstream title;
	title << "Channel " << settings.channel;
	_hist.SetTitle(title.str().c_str());
	_interrupted=false;
	_thread.Run();
}

HistogramManager::~HistogramManager() {
	_interrupted = true;
	_client.disconnect();
	join(_thread);
}
