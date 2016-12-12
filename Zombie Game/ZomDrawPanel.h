#pragma once
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <string>
#include "Machine.h"

class ZomDrawPanel : public wxPanel
{
 
public:
	ZomDrawPanel(wxFrame* parent);
	void PaintNow();
 
protected:
	void PaintEvent(wxPaintEvent & evt);
	void Render(wxDC& dc);
	void DrawGrid(wxDC& dc);
    void DrawMachines(wxDC& dc);
    void AddPoint(wxPointList& pointList, const MachineState* testState);
    void DrawSideInformation(wxDC& dc);
 
	DECLARE_EVENT_TABLE()
	
public:
};

