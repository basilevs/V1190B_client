#ifndef GUI_H_
#define GUI_H_

#include "TGFrame.h"
class TCanvas;
class TGNumberEntry;
class TGTextEntry;
class TGStatusBar;
class TGCheckButton;
#ifndef __CINT__
#include "HistogramManager.h"
#endif

#include <Rtypes.h>
#include <TTimer.h>

class Gui: public TGMainFrame {
#ifndef __CINT__
	HistogramManager _histogram;
	void configure(const Settings & settings);
#endif
	TTimer _timer;
	TCanvas * _canvas;
	TGNumberEntry * _channel;
	TGNumberEntry * _triggerChannel;
	TGNumberEntry * _port;
	TGNumberEntry * _width;
	TGNumberEntry * _offset;
	TGTextEntry * _host;
	TGStatusBar * _status;
	TGCheckButton * _subtractTrigger;
	TGCheckButton * _runningAverage;
	TGCheckButton * _firstHit;
	void init();
	Bool_t HandleKey(Event_t * event);
public:
	void OnTimer();
	void ProcessSettings();
	Gui();
	ClassDef(Gui, 0)
};


#endif /* GUI_H_ */
