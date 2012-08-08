#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <string>

struct Settings {
	std::string host;
	unsigned port;
	bool showOnlyFirstHit;
	unsigned channel;
	unsigned windowWidth;
	int windowOffset;
	bool subtractTrigger;
	bool runningAverage;
	std::string check() const;
	Settings();
};

#include "V1190BClient.h"
void apply(const Settings & settings, V1190BClient & client);

#endif /* SETTINGS_H_ */
