#include "V1190BClient.h"
#include <iostream>
#include <assert.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include <TRint.h>


typedef V1190BClient::Event::Measurements Measurements;

using namespace std;

TBranch * createBracnh(TTree & tree, const char * name, int * ptr, unsigned size) {
	TString format;
	if (size == 0) 
		return 0;
	for (unsigned i = 0; i < size; ++i) {
		if (i == 0) {
			format += "ch00/I";
		} else {
			format += TString::Format(":ch%02u", i);
		}
			
	}
	cerr << "Branch format: " << format << endl;
	return tree.Branch(name, ptr, format);
}


int main(int argc, char* argv[]) {
	ostream * eventOutput = &cout;
	int events = 10000, offset = -2, width = 10;
	const static TString defRootFileName("V1190B.root");
	TString rootFileName = defRootFileName;
	bool interactive = false;
	
	int opt;
	while ((opt = getopt(argc, argv, "iho:n:f:w:")) != -1) {
		switch(opt) {
		case 'o': rootFileName = optarg; break;
		case 'i': interactive = true; break;
		case 'n': events = atoi(optarg); break;
		case 'f': offset = atoi(optarg); break;
		case 'w': width = atoi(optarg); break;
		case 'h':
		default: cerr <<
			argv[0] << " builds a ROOT tree with raw times read from V1190B. Address to read from is 127.0.0.1:1055\n" <<
			" -i        - start an interactive shell after data collection\n" << 
			" -n number - a number of events to collect. Default: 10000\n" << 
			" -f number - sets a window offset in ticks\n" <<
			" -w number - sets a window width in ticks\n" <<
			" -o file   - a name of a file to store ROOT tree. Default: " << defRootFileName  << "\n" << 
			" -h        - show help" <<
			endl;
			return 1;
		}
	}
	
	V1190BClient client("192.168.1.2", 1055);
	int rc;
	if ((rc = client.connect()) != socketwrapper::OK ) {
		cerr << "Failed to connect" << endl;
		return 5;
	}
	sleep(1);
	V1190BClient::Event event;
	client.setWindow(offset, width);
	long prevEvent = -1;

	TRint app("app", 0, 0, 0, 0, true);
	TFile file(rootFileName, "RECREATE");
	TTree *tree = new TTree("eventTree","rootEvent");

	const int BRANCH_SIZE = 64;
	int channels[BRANCH_SIZE];

	TBranch * branch = createBracnh(*tree, "rawTimes", channels, BRANCH_SIZE);

	if (!branch) {
		cerr << "Couldn't create branch" << endl;
		return 5;
	}
	
	while (client) {
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
		fill(channels, channels+BRANCH_SIZE, 0); 
		for (Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++) {
			if (i->gChannel() >= BRANCH_SIZE)
				continue;
			if (channels[i->gChannel()] > 0) {
				channels[i->gChannel()] = min(channels[i->gChannel()], i->time);
			} else {
				channels[i->gChannel()] = i->time;
			}
		}
		tree->Fill();
		if (long(event.count) > events)
			break;
			
	}
	file.Write();
	if (interactive)
		app.Run();

	return 0;
}
