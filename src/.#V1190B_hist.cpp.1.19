#include "V1190BClient.h"
#include <iostream>
#include <assert.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits>
#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include <TRint.h>
#include <HistogramManager.h>


typedef V1190BClient::Event::Measurements Measurements;

using namespace std;

const char * resolutionModeToPeriodStr(int mode) {
	if (mode < 0)
		return "100 ps";
	switch(mode) {
		case 3:
			return "100 ps";
		case 2: return "200 ps";
		case 1:
		case 0:
			return "800 ps";
		default:
			assert(!"Bad resolution");
			return "Bad resolution";
	}
}

bool interrupt = false;
static void sighandler(int s) {
//	cerr << "Signal " << s << " caught" << endl;
	interrupt = true;
}


int main(int argc, char* argv[]) {
	ostream * eventOutput = &cout;
	unsigned int events = 10000;
	int triggerChannel = 0;
	const static TString defRootFileName("V1190B_hist.root");
	TString rootFileName = defRootFileName;
	bool interactive = false;
	int resolutionMode = -1;
	int deadTime = -1, offset = -2, width = 10;

	int opt;
	while ((opt = getopt(argc, argv, "iho:n:t:qd:r:f:w:")) != -1) {
		switch(opt) {
		case 'o': rootFileName = optarg; break;
		case 'i': interactive = true; break;
		case 'n': events = atoi(optarg); break;
		case 't': triggerChannel = atoi(optarg); break;
		case 'q': eventOutput = 0; break;
		case 'd': deadTime = atoi(optarg); break;
		case 'r': resolutionMode = atoi(optarg); break;
		case 'f': offset = atoi(optarg); break;
		case 'w': width = atoi(optarg); break;
		case 'h':
		default: cerr <<
			argv[0] << " builds a ROOT tree with raw times read from V1190B. Address to read from is 127.0.0.1:1055\n" <<
			" -i        - start an interactive shell after data collection\n" <<
			" -n number - a number of events to collect. Default: " << events <<"\n" <<
			" -t number - a channel to take trigger time from. Default: " << triggerChannel << "\n" <<
			" -d number - dead time in nanoseconds. 5, 10, 30, 100 are allowed. Default: 5\n" <<
			" -f number - sets a trigger window offset in ticks\n" <<
			" -w number - sets a trigger window width in ticks (be aware, that window and offset are used together TDC failures might apper if invalid value pair is used)\n" <<
			" -r number - resolution mode. Accepted values: 0 (800 ps), 1(800 ps) 2(200 ps), 3 (100 ps). Default: 3 (100 ps).\n" <<
			" -o file   - a name of a file to store ROOT histograms. Default: " << defRootFileName  << "\n" <<
			" -q        - don't print events\n" <<
			" -h        - show help" <<
			endl;
			return 1;
		}
	}
	
	if (width < 1) {
		cerr << "Invalid window width: " << width << endl;
		return 1;
	}

	if (offset + width  > 40) {
		cerr << "Invalid offset: " << offset << endl;
		return 1;
	}

	V1190BClient client("192.168.1.2", 1055);

	int rc;
	if ((rc = client.connect()) != socketwrapper::OK ) {
		cerr << "Failed to connect" << endl;
		return 5;
	}

	sleep(1);
	client.setWindow(offset, width);
	if (deadTime >= 0)
		client.setDeadTime(deadTime);
	if (resolutionMode >= 0)
		client.setDllClock(resolutionMode);
	V1190BClient::Event event;
	long prevEvent = -1;

	TRint app("app", 0, 0, 0, 0, true);
	TFile file(rootFileName, "RECREATE");
	{ // file scope
	auto_ptr<TH1> hist(new TH1F("hist", "Time spectrum", 10000, -5000.-0.5, 5000.-0.5));
	string resolutionStr("Time, ");
	resolutionStr+=resolutionModeToPeriodStr(resolutionMode);
	hist->GetXaxis()->SetTitle(resolutionStr.c_str());
	HistogramManager hm(hist);

	if (signal(SIGINT, sighandler) == SIG_ERR) {
		cerr << "Signal handler is not installed Ctrl-C will drop the data collected" << endl;
	}
	while (client) {
		if (interrupt)
			break;
		if (!client.readEvent(event)) {
			if (client) cerr << "Bad event" << endl;
			continue;
		}
		if (eventOutput)
			*eventOutput << event << "\n";
		if (prevEvent+1 != long(event.count)) {
			cerr << "Event shift: from " << dec << prevEvent << " to " << event.count << endl;
		}
		prevEvent=event.count;
		const int invalid = numeric_limits<int>::min();
		int triggerTime = invalid;
		if (triggerChannel >= 0) {
			for (Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++)
				if (i->gChannel() == triggerChannel) {
					triggerTime = i->time;
					break;
				}
		} else {
			triggerTime = 0;
		}
		if (triggerTime != invalid) {
			for (Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++) {
				hm[i->gChannel()]->Fill(i->time-triggerTime);
			}
		} else {
			cerr << "No trigger in event " << event.count << endl;
		}

		if (event.count >= events)
			break;
	}
//	hm.Write();
	file.Write();
	} // file scope
	if (interactive)
		app.Run();

	return 0;
}
