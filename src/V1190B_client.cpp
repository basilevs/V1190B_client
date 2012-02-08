#include "V1190BClient.h"
#include <iostream>
#include <assert.h>
#include <unistd.h>


using namespace std;

int main() {
	V1190BClient client("192.168.1.2", 1055);
	int rc;
	if ((rc = client.connect()) != socketwrapper::OK ) {
		cerr << "Failed to connect" << endl;
		return 5;
	}
	V1190BClient::Event event;
	client.setWindow(-2, 10);
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
