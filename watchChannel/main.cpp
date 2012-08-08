#include "gui.h"
#include <TApplication.h>
#include <TSystem.h>

int main() {

	int argc = 0;
	char * argv[0];
	TApplication app("V1190B channel monitor", &argc, argv);

	TGMainFrame * gui;
	gui = new Gui();
//	gui = new TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame);
	gui->Connect("CloseWindow()", "TSystem", gSystem, "ExitLoop()");
	app.Run();
	return 0;
}
