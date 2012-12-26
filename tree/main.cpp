#include "V1190BClient.h"
#include <iostream>
#include <sstream>
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

TBranch * createBranch(TTree & tree, const char * name, int * ptr, unsigned size) {
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

struct Runner {
	ostream * _output;
	TString _rootFileName;
	int _events, _offset, _width, _port;
	string _host;
	Runner() {
		_output = &cout;
		_events = 10000;
		_offset = -2;
		_width = 10;
		_rootFileName = "V1190B.root";
		_host = "192.168.1.2";
		_port = 1055;
	}
	void collectInteractive() {
		TRint app("app", 0, 0, 0, 0, true);
		collect();
		app.Run();
	}
	int collect() {
		V1190BClient client(_host.c_str(), _port);
		int rc;
		client.connect();
		sleep(1);
		V1190BClient::Event event;
		client.setWindow(_offset, _width);
		long prevEvent = -1;

		TFile file(_rootFileName, "RECREATE");
		TTree *tree = new TTree("eventTree","rootEvent");

		const int BRANCH_SIZE = 64;
		int firstTimes[BRANCH_SIZE];
		Int_t channels[BRANCH_SIZE];
		Int_t times[BRANCH_SIZE];
		int hitCount;


		TBranch * branch = createBranch(*tree, "firstTimes", firstTimes, BRANCH_SIZE);

		if (!branch) {
			cerr << "Couldn't create first times branch" << endl;
			return 5;
		}

		branch = tree->Branch("hitCount", &hitCount, "hitCount/I");
		if (!branch) {
			cerr << "Couldn't create hitCount branch" << endl;
			return 5;
		}

		branch = tree->Branch("channels", channels, "channels[hitCount]/I");
		if (!branch) {
			cerr << "Couldn't create channels branch" << endl;
			return 5;
		}

		branch = tree->Branch("times", times, "times[hitCount]/I");
		if (!branch) {
			cerr << "Couldn't create times branch" << endl;
			return 5;
		}



		while (client) {
			if (!client.readEvent(event)) {
				if (client) cerr << "Bad event" << endl;
				continue;
			}
			if (_output)
				*_output << event << "\n";
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

			hitCount = 0;
			for (Measurements::const_iterator i=event.measurements.begin(); i!=event.measurements.end(); i++) {
				if (hitCount >= BRANCH_SIZE) {
					cerr << "Event is too big" << endl;
					break;
				}
				channels[hitCount] = i->gChannel();
				times[hitCount] = i->time;
				hitCount++;
			}
			tree->Fill();
			if (long(event.count) > _events)
				break;

		}
		file.Write();
		return 0;
	}
};


int main(int argc, char* argv[]) {
	bool interactive = false;
	int opt;
	Runner runner;
	while ((opt = getopt(argc, argv, "iho:n:f:w:a:")) != -1) {
		switch(opt) {
		case 'o': runner._rootFileName = optarg; break;
		case 'i': interactive = true; break;
		case 'n': runner._events = atoi(optarg); break;
		case 'f': runner._offset = atoi(optarg); break;
		case 'w': runner._width = atoi(optarg); break;
		case 'a':
		{
			istringstream address(optarg);
			getline(address, runner._host, ':') >> runner._port;
			if (address.fail()) {
				cerr << " Failed to parse address " << optarg << endl;
				return 7;
			}
		}
			break;
		case 'h':
		default: cerr <<
			argv[0] << " builds a ROOT tree with raw times read from V1190B.\n"
			" -a hostname:port - address to read from (default: " << runner._host << ":" << runner._port << ")\n" <<
			" -i        - start an interactive shell after data collection\n" << 
			" -n number - a number of events to collect. Default: 10000\n" << 
			" -f number - sets a window offset in ticks\n" <<
			" -w number - sets a window width in ticks\n" <<
			" -o file   - a name of a file to store ROOT tree. Default: " << "V1190B.root"  << "\n" <<
			" -h        - show help" <<
			endl;
			return 1;
		}
	}
	
	if (interactive)
		runner.collectInteractive();
	else
		return runner.collect();

	return 0;
}
