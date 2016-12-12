#include "ZomFrame.h"
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/filedlg.h>
#include "ZomDrawPanel.h"
#include <wx/wfstream.h>
#include "World.h"

enum
{
	ID_SImSTART=1000,
	ID_TURN_TIMER,
    ID_LOADZOMBIE,
    ID_LOADHUMAN,
    ID_RANDOM,
    ID_RESET
};

wxBEGIN_EVENT_TABLE(ZomFrame, wxFrame)
	EVT_MENU(wxID_EXIT, ZomFrame::OnExit)
	EVT_MENU(wxID_NEW, ZomFrame::OnNew)
    EVT_MENU(ID_LOADZOMBIE, ZomFrame::OnLoadZombie)
    EVT_MENU(ID_LOADHUMAN, ZomFrame::OnLoadHuman)
    EVT_MENU(ID_RANDOM, ZomFrame::OnRandom)
    EVT_MENU(ID_RESET, ZomFrame::OnReset)
	EVT_MENU(ID_SImSTART, ZomFrame::OnSimStart)
	EVT_TIMER(ID_TURN_TIMER, ZomFrame::OnTurnTimer)
wxEND_EVENT_TABLE()

ZomFrame::ZomFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
, mIsActive(false), mLoadZombieFinished(false), mLoadHumanFinished(false)
{
	// File menu
	menuFile = new wxMenu;
	menuFile->Append(wxID_NEW);
	menuFile->Append(wxID_EXIT);
    menuFile->Append(ID_LOADHUMAN, "Load Human ZOM file");
    menuFile->Append(ID_LOADZOMBIE, "Load Zombie ZOM file");
	
	// Simulation menu
	mSimMenu = new wxMenu;
	mSimMenu->Append(ID_SImSTART, "Start/stop\tSpace", "Start or stop the simulation");
    mSimMenu->Append(ID_RANDOM, "Randomize", "Add 10 zombies in random locations");
    mSimMenu->Append(ID_RESET, "Reset", "Reset month and machines");
    
    //Disable options
    ActivateSimMenu(false);

	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(mSimMenu, "&Simulation");
	SetMenuBar(menuBar);
	CreateStatusBar();
	
	// Prepare the draw panel and show this frame
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	mPanel = new ZomDrawPanel(this);
	sizer->Add(mPanel, 1, wxEXPAND);
	
	SetSizer(sizer);
	SetAutoLayout(true);
	Show(true);
	
	mTurnTimer = new wxTimer(this, ID_TURN_TIMER);
}

ZomFrame::~ZomFrame()
{
	delete mTurnTimer;
}

void ZomFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void ZomFrame::OnNew(wxCommandEvent& event)
{
    World::get().UnloadFP();
    mLoadHumanFinished = false;
    mLoadZombieFinished = false;
    ActivateSimMenu(false);
    OnReset(event);
}

void ZomFrame::OnSimStart(wxCommandEvent& event)
{
	if (!mIsActive)
	{
		// Add timer to run once per second
		mTurnTimer->Start(1000);
		mIsActive = true;
        menuFile->Enable(ID_LOADHUMAN, false);
        menuFile->Enable(ID_LOADZOMBIE, false);
	}
	else
	{
		mTurnTimer->Stop();
		mIsActive = false;
        menuFile->Enable(ID_LOADHUMAN, true);
        menuFile->Enable(ID_LOADZOMBIE, true);
	}
}

void ZomFrame::OnTurnTimer(wxTimerEvent& event)
{
    for (int i=0; i<World::get().GetZombieCount(); i++)
    {
        try
        {
            World::get().GetZombieMachine(i)->TakeTurn(*(World::get().GetZombieState(i)));
        }
        catch(InvalidRangedAttack ira)
        {
            wxMessageBox("Zombies cannot use ranged attacks!", "Error", wxOK | wxICON_ERROR);
            TimerReset();
            return;
        }
        catch(LineMissing lm)
        {
            wxMessageBox("Attempted to jump to non-existent line.", "Error", wxOK | wxICON_ERROR);
            TimerReset();
            return;
        }
    }
    
    for (int i=0; i<World::get().GetHumanCount(); i++)
    {
        try
        {
            World::get().GetHumanMachine(i)->TakeTurn(*(World::get().GetHumanState(i)));
        }
        catch(LineMissing lm)
        {
            wxMessageBox("Attempted to jump to non-existent line.", "Error", wxOK | wxICON_ERROR);
            TimerReset();
            return;
        }
    }
    
    //Declare a winner if necessary
    if (World::get().GetHumanCount() == 0)
    {
        mPanel->PaintNow();
        mTurnTimer->Stop();
		mIsActive = false;
        wxMessageBox("Zombies win!", "Game Finished", wxOK | wxICON_INFORMATION);
        TimerReset();
    }
    else if (World::get().GetZombieCount() == 0)
    {
        mPanel->PaintNow();
        mTurnTimer->Stop();
		mIsActive = false;
        wxMessageBox("Humans win!", "Game Finished", wxOK | wxICON_INFORMATION);
        TimerReset();
    }
    else
    {
        World::get().IncrementMonth();
    }
    
    mPanel->PaintNow();
}

void ZomFrame::OnLoadHuman(wxCommandEvent &event)
{
    //Create file dialog
    wxFileDialog
        openFileDialog(this, _("Open Human ZOM file"), "./zom", "",
                       "ZOM file (*.zom)|*.zom", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }
    
    wxFileInputStream input_stream(openFileDialog.GetPath());
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
        return;
    }
    else
    {
        try
        {
            World::get().LoadHumanFile(openFileDialog.GetPath().ToStdString());
            mLoadHumanFinished = true;
            
            if (mLoadZombieFinished)
            {
                ActivateSimMenu(true);
            }
            mPanel->PaintNow();
        }
        catch(FileLoadExcept fe)
        {
            wxMessageBox("ZOM file load failed", "Error", wxOK | wxICON_ERROR);
        }
        catch(InvalidOp io)
        {
            wxMessageBox("Invalid Op found. File is invalid", "Error", wxOK | wxICON_ERROR);
        }
    }
}

void ZomFrame::OnLoadZombie(wxCommandEvent &event)
{
    //Create file dialog
    wxFileDialog
        openFileDialog(this, _("Open Zombie ZOM file"), "./zom", "",
                       "ZOM file (*.zom)|*.zom", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    
    if (openFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }
    
    wxFileInputStream input_stream(openFileDialog.GetPath());
    if (!input_stream.IsOk())
    {
        wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
        return;
    }
    else
    {
        try
        {
            World::get().LoadZombieFile(openFileDialog.GetPath().ToStdString());
            mLoadZombieFinished = true;
            
            if (mLoadHumanFinished)
            {
                ActivateSimMenu(true);
            }
            mPanel->PaintNow();
        }
        catch(FileLoadExcept fe)
        {
            wxMessageBox("ZOM file load failed", "Error", wxOK | wxICON_ERROR);
        }
        catch(InvalidOp io)
        {
            wxMessageBox("Invalid Op found. File is invalid", "Error", wxOK | wxICON_ERROR);
        }

    }
}

void ZomFrame::OnRandom(wxCommandEvent &event)
{
    MachineState *tempMS;
    int tempX, tempY;
    srand(static_cast<unsigned>(std::time(0)));
    World::get().ClearAll();

    for (int i=0; i<30; i++)
    {
        if (i<20)
        {
            World::get().AddNewZombie();
            tempMS = World::get().GetZombieState(i);
        }
        else
        {
            World::get().AddNewHuman();
            tempMS = World::get().GetHumanState(i-20);
        }
        
        do
        {
            tempX = rand()%20;
            tempY = rand()%20;
        }
        while (World::get().CheckOccupiedTile(tempX, tempY));
        
        tempMS->mX = tempX;
        tempMS->mY = tempY;
        
        tempX = rand()%4;
        
        switch(tempX)
        {
        case 0:
            tempMS->mFacing = MachineState::UP;
            break;
        case 1:
            tempMS->mFacing = MachineState::RIGHT;
            break;
        case 2:
            tempMS->mFacing = MachineState::DOWN;
            break;
        case 3:
            tempMS->mFacing = MachineState::LEFT;
            break;
        }
    }
    mPanel->PaintNow();
}

void ZomFrame::OnReset(wxCommandEvent &event)
{
    //Reset data
    World::get().ClearAll();
    menuFile->Enable(ID_LOADHUMAN, true);
    menuFile->Enable(ID_LOADZOMBIE, true);
    
    //Stop timer
    mTurnTimer->Stop();
    mIsActive = false;
    mPanel->PaintNow();
}

void ZomFrame::ActivateSimMenu(bool on)
{
    mSimMenu->Enable(ID_RESET, on);
    mSimMenu->Enable(ID_RANDOM, on);
    mSimMenu->Enable(ID_SImSTART, on);
}

void ZomFrame::TimerReset()
{
    World::get().UnloadFP();
    mLoadHumanFinished = false;
    mLoadZombieFinished = false;
    ActivateSimMenu(false);
    
    //Reset data
    World::get().ClearAll();
    menuFile->Enable(ID_LOADHUMAN, true);
    menuFile->Enable(ID_LOADZOMBIE, true);
    
    //Stop timer
    mTurnTimer->Stop();
    mIsActive = false;
    mPanel->PaintNow();
}
