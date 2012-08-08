#ifndef HISTOGRAM_MANAGER_H_
#define HISTOGRAM_MANAGER_H_

#include "TH1.h"
#include "TThread.h"

#include "settings.h"
#include "V1190BClient.h"
class HistogramManager {
	Settings _settings;
	V1190BClient _client;
	TH1I _hist;
	volatile bool _interrupted;
	TThread _thread;
	static void * threadFunc(void *);
	void tryApply(const Settings & settings);
	HistogramManager(const HistogramManager &);
	HistogramManager operator=(const HistogramManager &);
public:
	HistogramManager();
	~HistogramManager();
	void apply(const Settings & settings);
	TH1 & histogram() {return _hist;}
	Settings settings() const {return _settings;}
};
#endif

