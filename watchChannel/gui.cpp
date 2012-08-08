// Mainframe macro generated from application: /usr/bin/root.exe
// By ROOT version 5.30/04 on 2012-08-01 19:52:42

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
#ifndef ROOT_TGMdiDecorFrame
#include "TGMdiDecorFrame.h"
#endif
#ifndef ROOT_TG3DLine
#include "TG3DLine.h"
#endif
#ifndef ROOT_TGMdiFrame
#include "TGMdiFrame.h"
#endif
#ifndef ROOT_TGMdiMainFrame
#include "TGMdiMainFrame.h"
#endif
#ifndef ROOT_TGMdiMenu
#include "TGMdiMenu.h"
#endif
#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TGComboBox
#include "TGComboBox.h"
#endif
#ifndef ROOT_TGuiBldHintsEditor
#include "TGuiBldHintsEditor.h"
#endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGFileDialog
#include "TGFileDialog.h"
#endif
#ifndef ROOT_TGShutter
#include "TGShutter.h"
#endif
#ifndef ROOT_TGButtonGroup
#include "TGButtonGroup.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
#ifndef ROOT_TGuiBldEditor
#include "TGuiBldEditor.h"
#endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
#ifndef ROOT_TGFSComboBox
#include "TGFSComboBox.h"
#endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
#ifndef ROOT_TGMsgBox
#include "TGMsgBox.h"
#endif
#ifndef ROOT_TRootGuiBuilder
#include "TRootGuiBuilder.h"
#endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
#ifndef ROOT_TGSplitter
#include "TGSplitter.h"
#endif
#ifndef ROOT_TGStatusBar
#include "TGStatusBar.h"
#endif
#ifndef ROOT_TGListTree
#include "TGListTree.h"
#endif

#ifndef ROOT_TGToolTip
#include "TGToolTip.h"
#endif
#ifndef ROOT_TGToolBar
#include "TGToolBar.h"
#endif
#ifndef ROOT_TRootEmbeddedCanvas
#include "TRootEmbeddedCanvas.h"
#endif
#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#include "gui.h"

#include "boost/foreach.hpp"
#include <stdexcept>
#include <iostream>

#include "KeySymbols.h"

using namespace std;


void Gui::configure(const Settings & s) {
	_channel->SetIntNumber(s.channel);
	_port->SetIntNumber(s.port);
	_offset->SetIntNumber(s.windowOffset);
	_width->SetIntNumber(s.windowWidth);
	_host->SetText(s.host.c_str(), false);
	_runningAverage->SetOn(s.runningAverage, false);
	_firstHit->SetOn(s.showOnlyFirstHit, false);
	_subtractTrigger->SetOn(s.subtractTrigger, false);
}

void Gui::ProcessSettings()
{
	Settings s;
	s.channel = (unsigned)_channel->GetNumber();
	s.host = _host->GetText();
	s.port = (unsigned)_port->GetNumber();
	s.runningAverage = _runningAverage->IsOn();
	s.showOnlyFirstHit = _firstHit->IsOn();
	s.subtractTrigger = _subtractTrigger->IsOn();
	s.windowOffset = _offset->GetNumber();
	s.windowWidth = _width->GetNumber();
	try {
		_histogram.apply(s);
		configure(s);
		if (_status)
			_status->SetText("I'm happy", 0);
		cerr << "I'm happy" << endl;
	} catch(runtime_error & e) {
		if (_status)
			_status->SetText(e.what(), 0);
		cerr << e.what() << endl;
	}
}

Gui::Gui():
	TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame),
	_timer(1000)
{
	TGMainFrame * fMainFrame1997 = this;
	   fMainFrame1997->SetName("fMainFrame1997");
	   TGVerticalFrame *fVerticalFrame556 = new TGVerticalFrame(fMainFrame1997,545,470,kVerticalFrame);
	   fVerticalFrame556->SetName("fVerticalFrame556");

	   // embedded canvas
	   TRootEmbeddedCanvas *canvas = new TRootEmbeddedCanvas(0,fVerticalFrame556,541,362);
	   canvas->SetName("canvas");
	   Int_t wcanvas = canvas->GetCanvasWindowId();
	   TCanvas *c125 = new TCanvas("c125", 10, 10, wcanvas);
	   canvas->AdoptCanvas(c125);
	   fVerticalFrame556->AddFrame(canvas, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
	   TGStatusBar *fStatusBar1313 = 0;
#if 0
	   // status bar
	   fStatusBar1313 = new TGStatusBar(fMainFrame1997,545,18);
	   fStatusBar1313->SetName("fStatusBar1313");
	   fMainFrame1997->AddFrame(fStatusBar1313, new TGLayoutHints(kLHintsLeft | kLHintsBottom | kLHintsExpandX));
#endif

#if 1
	   // horizontal frame
	   TGHorizontalFrame *fHorizontalFrame585 = new TGHorizontalFrame(fVerticalFrame556,524,86,kHorizontalFrame | kRaisedFrame);
	   fHorizontalFrame585->SetName("fHorizontalFrame585");

#if 1
	   // vertical frame
	   TGVerticalFrame *fVerticalFrame592 = new TGVerticalFrame(fHorizontalFrame585,125,57,kVerticalFrame);
	   fVerticalFrame592->SetName("fVerticalFrame592");
	   TGCheckButton *subtractTrigger = new TGCheckButton(fVerticalFrame592,"Subtract trigger");
	   subtractTrigger->SetTextJustify(36);
	   subtractTrigger->SetMargins(0,0,0,0);
	   subtractTrigger->SetWrapLength(-1);
	   fVerticalFrame592->AddFrame(subtractTrigger, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   TGCheckButton *firstHit = new TGCheckButton(fVerticalFrame592,"First hit only");
	   firstHit->SetTextJustify(36);
	   firstHit->SetMargins(0,0,0,0);
	   firstHit->SetWrapLength(-1);
	   fVerticalFrame592->AddFrame(firstHit, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   TGCheckButton *runningAverage = new TGCheckButton(fVerticalFrame592,"Running average");
	   runningAverage->SetTextJustify(36);
	   runningAverage->SetMargins(0,0,0,0);
	   runningAverage->SetWrapLength(-1);
	   fVerticalFrame592->AddFrame(runningAverage, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

	   fHorizontalFrame585->AddFrame(fVerticalFrame592, new TGLayoutHints(kLHintsNormal));
#endif

#if 1
	   // vertical frame
	   TGVerticalFrame *fVerticalFrame717 = new TGVerticalFrame(fHorizontalFrame585,153,80,kVerticalFrame);
	   fVerticalFrame717->SetName("fVerticalFrame717");

	   // horizontal frame
	   TGHorizontalFrame *fHorizontalFrame726 = new TGHorizontalFrame(fVerticalFrame717,153,24,kHorizontalFrame);
	   fHorizontalFrame726->SetName("fHorizontalFrame726");
	   TGLabel *fLabel1700 = new TGLabel(fHorizontalFrame726,"Channel:");
	   fLabel1700->SetTextJustify(36);
	   fLabel1700->SetMargins(0,0,0,0);
	   fLabel1700->SetWrapLength(-1);
	   fHorizontalFrame726->AddFrame(fLabel1700, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   TGNumberEntry *channel = new TGNumberEntry(fHorizontalFrame726, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
	   channel->SetName("channel");
	   fHorizontalFrame726->AddFrame(channel, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));

	   fVerticalFrame717->AddFrame(fHorizontalFrame726, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX));

	   // horizontal frame
	   TGHorizontalFrame *fHorizontalFrame794 = new TGHorizontalFrame(fVerticalFrame717,149,24,kHorizontalFrame);
	   fHorizontalFrame794->SetName("fHorizontalFrame794");
	   TGLabel *fLabel1706 = new TGLabel(fHorizontalFrame794,"Window width:");
	   fLabel1706->SetTextJustify(36);
	   fLabel1706->SetMargins(0,0,0,0);
	   fLabel1706->SetWrapLength(-1);
	   fHorizontalFrame794->AddFrame(fLabel1706, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   TGNumberEntry *width = new TGNumberEntry(fHorizontalFrame794, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
	   width->SetName("width");
	   fHorizontalFrame794->AddFrame(width, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));

	   fVerticalFrame717->AddFrame(fHorizontalFrame794, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));

	   // horizontal frame
	   TGHorizontalFrame *fHorizontalFrame859 = new TGHorizontalFrame(fVerticalFrame717,149,24,kHorizontalFrame);
	   fHorizontalFrame859->SetName("fHorizontalFrame859");
	   TGLabel *fLabel1712 = new TGLabel(fHorizontalFrame859,"Window offset:");
	   fLabel1712->SetTextJustify(36);
	   fLabel1712->SetMargins(0,0,0,0);
	   fLabel1712->SetWrapLength(-1);
	   fHorizontalFrame859->AddFrame(fLabel1712, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   TGNumberEntry *offset = new TGNumberEntry(fHorizontalFrame859, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
	   offset->SetName("offset");
	   fHorizontalFrame859->AddFrame(offset, new TGLayoutHints(kLHintsRight | kLHintsTop,2,2,2,2));

	   fVerticalFrame717->AddFrame(fHorizontalFrame859, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));

	   fHorizontalFrame585->AddFrame(fVerticalFrame717, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
#endif

#if 1
	   // vertical frame
	   TGVerticalFrame *fVerticalFrame888 = new TGVerticalFrame(fHorizontalFrame585,240,48,kVerticalFrame);
	   fVerticalFrame888->SetName("fVerticalFrame888");
	   fVerticalFrame888->SetLayoutManager(new TGMatrixLayout(fVerticalFrame888,2,1,0,0));

	   // horizontal frame
	   TGHorizontalFrame *fHorizontalFrame951 = new TGHorizontalFrame(fVerticalFrame888,240,24,kHorizontalFrame);
	   fHorizontalFrame951->SetName("fHorizontalFrame951");
	   TGLabel *fLabel1719 = new TGLabel(fHorizontalFrame951,"Host:");
	   fLabel1719->SetTextJustify(36);
	   fLabel1719->SetMargins(0,0,0,0);
	   fLabel1719->SetWrapLength(-1);
	   fHorizontalFrame951->AddFrame(fLabel1719, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

	   TGFont *ufont;         // will reflect user font changes
	   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

	   TGGC   *uGC;           // will reflect user GC changes
	   // graphics context changes
	   GCValues_t valEntry1720;
	   valEntry1720.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
	   gClient->GetColorByName("#000000",valEntry1720.fForeground);
	   gClient->GetColorByName("#e7e3e7",valEntry1720.fBackground);
	   valEntry1720.fFillStyle = kFillSolid;
	   valEntry1720.fFont = ufont->GetFontHandle();
	   valEntry1720.fGraphicsExposures = kFALSE;
	   uGC = gClient->GetGC(&valEntry1720, kTRUE);
	   TGTextEntry *host = new TGTextEntry(fHorizontalFrame951, new TGTextBuffer(31),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kDoubleBorder | kOwnBackground);
	   host->SetMaxLength(4096);
	   host->SetAlignment(kTextLeft);
	   host->SetText("fTextEntry965");
	   host->Resize(202,host->GetDefaultHeight());
	   fHorizontalFrame951->AddFrame(host, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));

	   fVerticalFrame888->AddFrame(fHorizontalFrame951, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

	   // horizontal frame
	   TGHorizontalFrame *fHorizontalFrame974 = new TGHorizontalFrame(fVerticalFrame888,95,24,kHorizontalFrame);
	   fHorizontalFrame974->SetName("fHorizontalFrame974");
	   TGLabel *fLabel1722 = new TGLabel(fHorizontalFrame974,"Port:");
	   fLabel1722->SetTextJustify(36);
	   fLabel1722->SetMargins(0,0,0,0);
	   fLabel1722->SetWrapLength(-1);
	   fHorizontalFrame974->AddFrame(fLabel1722, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
	   TGNumberEntry *port = new TGNumberEntry(fHorizontalFrame974, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
	   port->SetName("port");
	   fHorizontalFrame974->AddFrame(port, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

	   fVerticalFrame888->AddFrame(fHorizontalFrame974, new TGLayoutHints(kLHintsNormal));

	   fHorizontalFrame585->AddFrame(fVerticalFrame888, new TGLayoutHints(kLHintsNormal));
#endif

	   fVerticalFrame556->AddFrame(fHorizontalFrame585, new TGLayoutHints(kLHintsBottom));
#endif
	   fMainFrame1997->AddFrame(fVerticalFrame556, new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY,1,1,1,1));
	   fMainFrame1997->MoveResize(0,0,553,519);

	   fMainFrame1997->SetMWMHints(kMWMDecorAll,
	                        kMWMFuncAll,
	                        kMWMInputModeless);
	   fMainFrame1997->MapSubwindows();

	   fMainFrame1997->Resize(fMainFrame1997->GetDefaultSize());
	   fMainFrame1997->MapWindow();
	   fMainFrame1997->Resize(562,515);

#if 1
	   _canvas = canvas->GetCanvas();
	   _channel = channel;
	   _port = port;
	   _width = width;
	   _offset = offset;
	   _host = host;
	   _status = fStatusBar1313;
	   _subtractTrigger = subtractTrigger;
	   _runningAverage = runningAverage;
	   _firstHit = firstHit;
	   init();
#endif
}

void Gui::init() {
	TGNumberEntry * numbers[] = {_channel, _port, _width, _offset};
	BOOST_FOREACH(TGNumberEntry * _entry, numbers) {
		_entry->Connect("ValueSet(Long_t)", "Gui", this, "ProcessSettings()");
	}
	_host->Connect("TextChanged(const char *)", "Gui", this, "ProcessSettings()");
	TGCheckButton * checks[] = {_firstHit, _subtractTrigger, _runningAverage};
	BOOST_FOREACH(TGCheckButton * _entry, checks) {
		_entry->Connect("Toggled(Bool_t)", "Gui", this, "ProcessSettings()");
	}
	configure(_histogram.settings());
	_canvas->cd();
	_histogram.histogram().Draw();
	_timer.Connect("Timeout()", "Gui", this, "OnTimer()");
	_timer.Start(1000);
	gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_Escape), 0, kTRUE);
}

void Gui::OnTimer() {
	_canvas->Modified();
	_canvas->Update();
}

Bool_t Gui::HandleKey(Event_t * event)
{
//	cerr << "Keyboard event: " << *event << endl;
	if (event->fType== kGKeyPress && event->fState == 0) {
		UInt_t keysym;
		char str[2];
		gVirtualX->LookupString(event, str, sizeof(str), keysym);
		switch(keysym) {
		case kKey_Escape:
			SendCloseMessage();
			return true;
		}
	}
	return TGMainFrame::HandleKey(event);
}

