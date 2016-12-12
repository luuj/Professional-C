#pragma once
#include <wx/frame.h>
#include <wx/timer.h>
#include <wx/menu.h>
#include "Machine.h"
#include "Traits.h"

class ZomFrame : public wxFrame
{
public:
	ZomFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~ZomFrame();
private:
	void OnExit(wxCommandEvent& event);
	void OnNew(wxCommandEvent& event);
	void OnSimStart(wxCommandEvent& event);
	void OnTurnTimer(wxTimerEvent& event);
	void OnLoadZombie(wxCommandEvent& event);
    void OnLoadHuman(wxCommandEvent& event);
    void OnRandom(wxCommandEvent& event);
    void OnReset(wxCommandEvent& event);
    void ActivateSimMenu(bool on);
    void TimerReset();
    
	wxDECLARE_EVENT_TABLE();
private:
	// Panel for drawing
	class ZomDrawPanel* mPanel;
	
	// Simulation menu
	wxMenu* mSimMenu;
    wxMenu* menuFile;
	
	// Turn timer
	wxTimer* mTurnTimer;
	
	bool mIsActive;
    
    bool mLoadZombieFinished, mLoadHumanFinished;
};
