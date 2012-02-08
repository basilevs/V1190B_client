#include "TH1.h"
#include "TRint.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "V1190BClient.h"
#include "HistogramManager.h"
#include <iostream>
#include <sstream>
#include <map>
#include <time.h>
#include <assert.h>

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

int main(int argc, char * argv[]) {
	TRint app("app", &argc, argv, 0, 0, true);
	V1190BClient client("127.0.0.1", 1055);
	V1190BClient::Event event;
	auto_ptr<TH1> hist(new TH1F("hist", "Time spectrum", 1000, -0.5, 500-0.5));
	hist->GetXaxis()->SetTitle("Time, ns");
	HistogramManager hm(hist);
	TCanvas canvas;
	time_t lastUpdate=time(0), lastOffsetChange=time(0);
	int offset=0;
	unsigned long prevEvent=0;
	while (client) {
		if (client.readEvent(event)) {
			//			cout << event << "\n";
			for (HistogramManager::iterator i=hm.begin(); i!=hm.end(); i++) {
				TH1 * h=i->second;
				assert(h);
				for (int i=1; i<=h->GetNbinsX(); i++) {
					double temp=h->GetBinContent(i);
					h->SetBinContent(i, temp*0.9997);
				}}
			typedef V1190BClient::Event::Measurements Measurements;
			for (Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++) {
				clog << i->gChannel() << ":" << i->time << endl;
				hm[i->gChannel()]->Fill(0.1*i->time);
			}
		} else {
			if (client) cout << "Bad event" << endl;
		}
		time_t currentTime=time(0);
		if (currentTime>lastUpdate+1) {
			for (HistogramManager::iterator i=hm.begin(); i!=hm.end(); i++) {
				assert(i->second);
				TCanvas * canvas=createCanvas(*i->second);
				assert(canvas);
				canvas->cd();
				i->second->Draw();
				canvas->Update();
				canvas->Modified();
			}
			lastUpdate=currentTime;
		}
		if (currentTime>lastOffsetChange+30) {
			lastOffsetChange=currentTime;
			offset+=2;
			clog << "Offset: "<< offset << endl;
			client.setWindow(offset, 10);
			//			clog << "Clear" << endl;
			//			client.clear();
		}
		if (prevEvent+1!=event.count) {
			cerr << "Event shift: from " << dec << prevEvent << " to " << event.count << endl;
		}
		prevEvent=event.count;
	}
	return 0;
}
