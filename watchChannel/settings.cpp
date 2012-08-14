#include "settings.h"

using namespace std;

Settings::Settings() {
	port = 0;
	showOnlyFirstHit = true;
	channel = 1;
	windowOffset = 0;
	windowWidth = 1;
	subtractTrigger = true;
	runningAverage = false;
	triggerChannel = 0;
}

string Settings::check() const {
	if (host.empty())
		return "Hostname is not set";
	if (windowOffset + windowWidth > 40)
		return "Offset + width value is too big";
	return "";
}

void apply(const Settings & s, V1190BClient & client)
{
	int rc = client.connect(s.host.c_str(), s.port);
	if (rc != socketwrapper::OK)
		return;
	client.setWindow(s.windowOffset, s.windowWidth);
}


