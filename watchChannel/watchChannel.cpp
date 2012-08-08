#include "TH1.h"
#include "TRint.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "V1190BClient.h"
#include <getopt.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <time.h>
#include <assert.h>
#include <stdlib.h>

#include "settings.h"

using namespace std;

//Finds a canvas with an object
TCanvas* getCanvas(const TObject * object) {
	if (object==NULL) return NULL;
	if (!gROOT) return NULL;
	TSeqCollection * canvases=gROOT->GetListOfCanvases();
	if (!canvases) return NULL;
	TIterator * canvasIter=canvases->MakeIterator();
	TCanvas * canvas=NULL;
	while ( (canvas=dynamic_cast<TCanvas *>(canvasIter->Next())) ) {
		TIterator * primIter=canvas->GetListOfPrimitives()->MakeIterator();
		TObject * prim=NULL;
		while ((prim=primIter->Next())) {
			if (object == prim) {
				return canvas;
			}
		}
	}
	return NULL;
}

TCanvas* createCanvas(const TNamed & named) {
	if (&named==NULL) return NULL;
	if (!gROOT) return NULL;
	TCanvas * canvas=NULL;
	try {
		ostringstream name;
		name << named.GetName() << "_c";
		TSeqCollection * canvases=gROOT->GetListOfCanvases();
		if (!canvases) return NULL;
		TIterator * canvasIter=canvases->MakeIterator();
		if (!canvasIter) return NULL;
		while ( (canvas=dynamic_cast<TCanvas *>(canvasIter->Next())) ) {
			if (name.str()==canvas->GetName()) {
				canvas->cd();
				return canvas;
			}
		}
		string title=named.GetTitle();
		canvas=new TCanvas(name.str().c_str(), title.c_str());
	} catch (...) {
		canvas=NULL;
	}
	return canvas;
}

float integral(TH1 & h, float start, float stop) {
	return h.Integral(h.FindBin(start), h.FindBin(stop));
}

float integralPrint(TH1 & h, float start, float stop) {
	float rv = integral(h, start, stop);
	cout << "Integral(" << h.GetName() << ", " << start<<".."<<stop <<") = " << rv << "\n";
	return rv;
}

typedef V1190BClient::Event::Error Error;
bool operator < (const Error & e1, const Error & e2) {
	#define check(field) if (e1.field() != e2.field()) return e1.field() < e2.field();
	check(type)
	check(tdc)
	check(group)
	return false;
	#undef check
}

int main(int argc, char * argv[]) {
	int channel = 28, windowOffset = -2, windowWidth = 10;
	unsigned long events = 0;
	bool reduce = false;
	bool substractTriggerTime = true, lowSpeedCoreClock = false, firstHitOnly = false;
	ostream * ostr = &cout;
	string host="127.0.0.1";
	unsigned port = 1055;


	int opt;
	while ((opt = getopt(argc, argv, "olshrw:f:c:n:a:")) != -1) {
		switch(opt) {
		case 'a':
		{
			istringstream temp(optarg);
			getline(temp, host, ':') >> opt;
			if (temp.fail()) {
				cerr << "Address format should be hostname:port" << endl;
				return 2;
			}
		}
		break;
		case 'l':
			lowSpeedCoreClock = true;
			break;
		case 'r':
			reduce = true;
			break;
		case 'f':
			windowOffset = atoi(optarg);
			break;
		case 'w':
			windowWidth = atoi(optarg);
			break;
		case 'c':
			channel = atoi(optarg);
			break;
		case 'n':
			events = atol(optarg);
			break;
		case 's':
			substractTriggerTime = false;
			break;
		case 'o':
			firstHitOnly = true;
			break;
		case 'p':
			ostr = 0;
			break;
		case 'h':
		default:
			cerr <<
			" -a hostname:port - address to connect to (default: "<< host << ":" << port << "\n"
			" -o        - only first hit is displayed on spectra\n" <<
			" -l        - makes TDC work on low speed core clock\n" <<
			" -n number - sets a number of event to collect \n" <<
			" -c number - sets a channel to watch\n" <<
			" -w number - sets a window width in ticks (25ns each)\n" <<
			" -f number - sets a window offset in ticks\n" <<
			" -s        - don't subtract trigger time\n" <<
			" -r        - makes older measurements less relevant making histograms dynamic\n" <<
			" -p        - suppress event printing\n" <<		
			" -h        - display this help" << endl;
			if (opt!='h') {
				cerr << "Unknown option: " << char(opt) << endl;
				return 1;
			}
			return 0;
		}
	}
	TRint app("app", 0, 0, 0, 0, true);
	V1190BClient client("192.168.1.2", 1055);

	int rc = 0;
	if ((rc = client.connect()) != socketwrapper::OK ) {
 		cerr << "Failed to connect" << endl;
		return 5;
	}

	sleep(1);

	if (lowSpeedCoreClock)		client.lowSpeedCoreClock();

	client.setWindow(windowOffset, windowWidth);

	V1190BClient::Event event;

	float binWidth = 0.1; //ns
	float histOffset = - 50 - 0.5 * binWidth;
	float histWidth = windowWidth * 25 - histOffset + 50; // ns
	int bins = int(histWidth / binWidth);

	auto_ptr<TH1> hist(new TH1F("hist", "Time spectrum", bins, histOffset, histOffset + binWidth * bins ));
	hist->GetXaxis()->SetTitle("Time, ns");

	string chanName;
	{   ostringstream ostr;
		ostr << "chan" << setw(2) << channel;
		chanName = ostr.str();
	}

	auto_ptr<TH1> triggerTime((TH1*)hist->Clone("trigger"));
	auto_ptr<TH1> channelTime((TH1*)hist->Clone(chanName.c_str()));

	typedef std::map <Error, int> ErrorCounts;
	ErrorCounts errorCounts;
	TH1 * hists[] = {triggerTime.get(), channelTime.get()};
	const int histsLen = 2;

	time_t lastUpdate=time(0);
	unsigned long prevEvent=0;

	while (client) {
		if (client.readEvent(event)) {
			if (prevEvent && prevEvent+1!=event.count) {
				cerr << "Event shift: from " << dec << prevEvent << " to " << event.count << endl;
				break;
			}
			prevEvent=event.count;

			if (events > 0 && event.count > events)
				break;

			if (reduce) {
				for (int i= 0; i < histsLen; i++) {
					TH1 * h = hists[i];
					assert(h);
					for (int i=1; i<=h->GetNbinsX(); i++) {
						double temp=h->GetBinContent(i);
						h->SetBinContent(i, temp*0.9997);
					}
				}
			}
			typedef V1190BClient::Event::Measurements Measurements;
			float trigger = -1;

			for (Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++) {
				if (i->gChannel()!=0) continue;
				if (trigger >= 0) {
					trigger = -1;
					break;
				}
				trigger = 0.1 * i->time;
			}

			if (ostr) *ostr  << event << "\n";

			typedef V1190BClient::Event::Errors Errors;
			for (Errors::const_iterator i=event.errors.begin(); i!=event.errors.end(); ++i) {
				errorCounts[*i]++;
			}

			if (substractTriggerTime && trigger < 0) continue;

			if (ostr) *ostr  << "trigger: " << trigger << "\n";

			triggerTime->Fill(trigger);
			vector<float> allTimes;
			for (Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++) {
				if (i->gChannel()!=channel) continue;
				allTimes.push_back(0.1 * i->time - (substractTriggerTime ? trigger : 0));
			}
			if (firstHitOnly) {
				vector<float>::const_iterator i = min_element(allTimes.begin(), allTimes.end());
				if (i!=allTimes.end())
					channelTime->Fill(*i);
			} else {
				for (vector<float>::const_iterator i = allTimes.begin(); i!=allTimes.end(); ++i) {
					channelTime->Fill(*i);
				}
			}
		} else {
			if (client) cout << "Bad event" << endl;
		}
		time_t currentTime=time(0);
		if (currentTime>lastUpdate+1) {
			for (int i= 0; i < histsLen; i++) {
				TH1 * h = hists[i];
				TCanvas * canvas = createCanvas(*h);
				assert(canvas);
				canvas->cd();
				h->Draw();
				canvas->Update();
				canvas->Modified();
			}
			lastUpdate=currentTime;
		}
	}

	if (errorCounts.size() > 0) {
		cout << "Errors:\n";
		for (ErrorCounts::const_iterator i = errorCounts.begin(); i != errorCounts.end(); ++i) {
			cout << i->first << ": " << i->second << "\n";
		}
	}
	integralPrint(*channelTime, 140, 150);
	app.Run();
	return 0;
}

