#include "V1190BClient.h"

#include <assert.h>
#include <unistd.h>
#include <stdlib.h> //atoi

#include <iostream>
#include <sstream>


using namespace std;

int main(int argc, char * argv[]) {
	int offset = -2, width = 10, port = 1055;
	string host = "127.0.0.1";
	int opt;
	while ((opt = getopt(argc, argv, "f:w:a:h")) != -1) {
		switch(opt) {
		case 'f': offset = atoi(optarg); break;
		case 'w': width = atoi(optarg); break;
		case 'a':
		{
			istringstream address(optarg);
			getline(address, host, ':') >> port;
			if (address.fail()) {
				cerr << " Failed to parse address " << optarg << endl;
				return 7;
			}

		}
		case 'h':
		default: cerr <<
			argv[0] << " dumps V1190B event on the standard output.\n"
			" -a hostname:port - address to read from (default: " << host << ":" << port << ")\n" <<
			" -f number - sets a window offset in ticks\n" <<
			" -w number - sets a window width in ticks\n" <<
			" -h        - show help" <<
			endl;
			return 1;
		}
	}

	V1190BClient client(host.c_str(), port);
	int rc;
	if ((rc = client.connect()) != socketwrapper::OK ) {
		cerr << "Failed to connect" << endl;
		return 5;
	}
	V1190BClient::Event event;
	client.setWindow(offset, width);
	long prevEvent=-1;
	sleep(1);
	event.clear();
	while (client) {
		if (client.readEvent(event)) {
			cout << event << endl;
			if ((unsigned long)(prevEvent+1) != event.count) {
				cerr << "Event shift: from " << dec << prevEvent << " to " << event.count << endl;
			}
			prevEvent=event.count;
		} else {
			if (client) cout << "Bad event" << endl;
		}
	}
	return 0;
}
