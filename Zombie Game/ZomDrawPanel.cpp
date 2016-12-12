#include "ZomDrawPanel.h"
#include <wx/dcclient.h>
#include <wx/sizer.h>

#include "World.h"

BEGIN_EVENT_TABLE(ZomDrawPanel, wxPanel)
	EVT_PAINT(ZomDrawPanel::PaintEvent)
END_EVENT_TABLE()

ZomDrawPanel::ZomDrawPanel(wxFrame* parent)
: wxPanel(parent)
{
	
}

void ZomDrawPanel::PaintEvent(wxPaintEvent & evt)
{
	wxPaintDC dc(this);
	Render(dc);
}

void ZomDrawPanel::PaintNow()
{
	wxClientDC dc(this);
	Render(dc);
}

void ZomDrawPanel::Render(wxDC& dc)
{
	// Clear
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
	
	// Draw the grid
	DrawGrid(dc);
    DrawMachines(dc);
    DrawSideInformation(dc);
}

void ZomDrawPanel::DrawGrid(wxDC& dc)
{
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawRectangle(10, 10, 600, 600);
    
    for (int i=1; i<20; i++)
    {
        dc.DrawLine(10, 10+30*i, 610, 10+30*i);
        dc.DrawLine(10+30*i, 10, 10+30*i, 610);
    }
}

void ZomDrawPanel::DrawMachines(wxDC& dc)
{
    wxPointList pointList;
    MachineState *testState;

    if (World::get().Empty())
    {
        return;
    }
    
    //Draw Zombies
    dc.SetPen(*wxRED_PEN);
    dc.SetBrush(*wxRED_BRUSH);
    for (int i=0; i<World::get().GetZombieCount(); i++)
    {
        testState = World::get().GetZombieState(i);

        AddPoint(pointList, testState);
        dc.DrawPolygon(&pointList);
        pointList.clear();
    }
    
    //Draw Humans
    dc.SetPen(*wxGREEN_PEN);
    dc.SetBrush(*wxGREEN_BRUSH);
    for (int i=0; i<World::get().GetHumanCount(); i++)
    {
        testState = World::get().GetHumanState(i);

        AddPoint(pointList, testState);
        dc.DrawPolygon(&pointList);
        pointList.clear();
    }
}

void ZomDrawPanel::AddPoint(wxPointList &pointList, const MachineState* testState)
{
    if (testState->mFacing == MachineState::DOWN)
    {
        pointList.Insert(new wxPoint(10+(30*testState->mX),10+(30*testState->mY)));
        pointList.Insert(new wxPoint(40+(30*testState->mX),10+(30*testState->mY)));
        pointList.Insert(new wxPoint(25+(30*testState->mX),40+(30*testState->mY)));
    }
    else if (testState->mFacing == MachineState::RIGHT)
    {
        pointList.Insert(new wxPoint(10+(30*testState->mX),10+(30*testState->mY)));
        pointList.Insert(new wxPoint(10+(30*testState->mX),40+(30*testState->mY)));
        pointList.Insert(new wxPoint(40+(30*testState->mX),25+(30*testState->mY)));
    }
    else if (testState->mFacing == MachineState::UP)
    {
        pointList.Insert(new wxPoint(40+(30*testState->mX),40+(30*testState->mY)));
        pointList.Insert(new wxPoint(10+(30*testState->mX),40+(30*testState->mY)));
        pointList.Insert(new wxPoint(25+(30*testState->mX),10+(30*testState->mY)));
    }
    else if (testState->mFacing == MachineState::LEFT)
    {
        pointList.Insert(new wxPoint(40+(30*testState->mX),10+(30*testState->mY)));
        pointList.Insert(new wxPoint(40+(30*testState->mX),40+(30*testState->mY)));
        pointList.Insert(new wxPoint(10+(30*testState->mX),25+(30*testState->mY)));
    }
}

void ZomDrawPanel::DrawSideInformation(wxDC &dc)
{
    dc.SetTextForeground(*wxRED);
    dc.DrawText("Zombies", 620, 15);
    dc.DrawText("Program: " + World::get().GetZombieFP(), 620, 30);
    dc.DrawText("Alive: " + std::to_string(World::get().GetZombieCount()), 620, 45);
    
    dc.SetTextForeground(*wxGREEN);
    dc.DrawText("Humans", 620, 90);
    dc.DrawText("Program: " + World::get().GetHumanFP(), 620, 105);
    dc.DrawText("Alive: " + std::to_string(World::get().GetHumanCount()), 620, 120);
    
    dc.SetTextForeground(*wxBLACK);
    dc.DrawText("Month: " + std::to_string(World::get().GetMonth()), 620, 165);
}

