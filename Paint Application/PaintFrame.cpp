#include "PaintFrame.h"
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/filedlg.h>
#include <wx/toolbar.h>
#include <wx/wfstream.h>
#include <wx/image.h>
#include <wx/colordlg.h>
#include <wx/textdlg.h>
#include <wx/filedlg.h>
#include "PaintDrawPanel.h"
#include "PaintModel.h"
#include <string>
#include <iostream>

wxBEGIN_EVENT_TABLE(PaintFrame, wxFrame)
	EVT_MENU(wxID_EXIT, PaintFrame::OnExit)
	EVT_MENU(wxID_NEW, PaintFrame::OnNew)
	EVT_MENU(ID_Import, PaintFrame::OnImport)
	EVT_TOOL(ID_Import, PaintFrame::OnImport)
	EVT_MENU(ID_Export, PaintFrame::OnExport)
	EVT_TOOL(ID_Export, PaintFrame::OnExport)
	EVT_MENU(wxID_UNDO, PaintFrame::OnUndo)
	EVT_TOOL(wxID_UNDO, PaintFrame::OnUndo)
	EVT_MENU(wxID_REDO, PaintFrame::OnRedo)
	EVT_TOOL(wxID_REDO, PaintFrame::OnRedo)
	EVT_MENU(ID_Unselect, PaintFrame::OnUnselect)
	EVT_MENU(ID_Delete, PaintFrame::OnDelete)
	EVT_MENU(ID_SetPenColor, PaintFrame::OnSetPenColor)
	EVT_MENU(ID_SetPenWidth, PaintFrame::OnSetPenWidth)
	EVT_MENU(ID_SetBrushColor, PaintFrame::OnSetBrushColor)
	// The different draw modes
	EVT_TOOL(ID_Selector, PaintFrame::OnSelectTool)
	EVT_TOOL(ID_DrawLine, PaintFrame::OnSelectTool)
	EVT_TOOL(ID_DrawEllipse, PaintFrame::OnSelectTool)
	EVT_TOOL(ID_DrawRect, PaintFrame::OnSelectTool)
	EVT_TOOL(ID_DrawPencil, PaintFrame::OnSelectTool)
wxEND_EVENT_TABLE()	

PaintFrame::PaintFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	// Initialize image handlers to support BMP, PNG, JPEG
	wxImage::AddHandler(new wxPNGHandler());
	wxImage::AddHandler(new wxJPEGHandler());

	SetupMenu();
	
	SetupToolbar();

	SetupModelAndView();

	Show(true);
	
	SetMinSize(GetSize());
	SetMaxSize(GetSize());
}

void PaintFrame::SetupMenu()
{
	// File menu
	mFileMenu = new wxMenu();
	mFileMenu->Append(wxID_NEW);
	mFileMenu->Append(ID_Export, "Export...",
		"Export current drawing to image file.");
	mFileMenu->AppendSeparator();
	mFileMenu->Append(ID_Import, "Import...",
		"Import image into file.");
	mFileMenu->Append(wxID_EXIT);

	// Edit menu
	mEditMenu = new wxMenu();
	mEditMenu->Append(wxID_UNDO);
	mEditMenu->Append(wxID_REDO);
	mEditMenu->AppendSeparator();
	mEditMenu->Append(ID_Unselect, "Unselect",
		"Unselect the current selection");
	mEditMenu->AppendSeparator();
	mEditMenu->Append(ID_Delete, "Delete\tDel",
		"Delete the current selection");
	
	mEditMenu->Enable(wxID_UNDO, false);
	mEditMenu->Enable(wxID_REDO, false);
	mEditMenu->Enable(ID_Unselect, false);
	mEditMenu->Enable(ID_Delete, false);

	// Colors menu
	mColorMenu = new wxMenu();
	mColorMenu->Append(ID_SetPenColor, "Pen Color...", "Set the pen color.");
	mColorMenu->Append(ID_SetPenWidth, "Pen Width...", "Set the pen width.");
	mColorMenu->AppendSeparator();
	mColorMenu->Append(ID_SetBrushColor, "Brush Color...", "Set brush color");

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append(mFileMenu, "&File");
	menuBar->Append(mEditMenu, "&Edit");
	menuBar->Append(mColorMenu, "&Colors");
	SetMenuBar(menuBar);
	CreateStatusBar();
}

void PaintFrame::SetupToolbar()
{
	// Create the toolbar
	mToolbar = CreateToolBar();
	mToolbar->AddTool(wxID_NEW, "New",
		wxBitmap("Icons/New.png", wxBITMAP_TYPE_PNG),
		"New");
	mToolbar->AddTool(ID_Export, "Export",
		wxBitmap("Icons/Save.png", wxBITMAP_TYPE_PNG),
		"Export");
	mToolbar->AddTool(ID_Import, "Import",
		wxBitmap("Icons/Import.png", wxBITMAP_TYPE_PNG),
		"Import");
	mToolbar->AddSeparator();

	mToolbar->AddTool(wxID_UNDO, "Undo",
		wxBitmap("Icons/Undo.png", wxBITMAP_TYPE_PNG),
		"Undo");
	mToolbar->AddTool(wxID_REDO, "Redo",
		wxBitmap("Icons/Redo.png", wxBITMAP_TYPE_PNG),
		"Redo");
	mToolbar->AddSeparator();

	mToolbar->AddTool(ID_Selector, "Selector",
		wxBitmap("Icons/Cursor.png", wxBITMAP_TYPE_PNG),
		"Selector", wxITEM_CHECK);
	mToolbar->AddTool(ID_DrawLine, "Draw Line",
		wxBitmap("Icons/Line.png", wxBITMAP_TYPE_PNG),
		"Draw Line", wxITEM_CHECK);
	mToolbar->AddTool(ID_DrawEllipse, "Draw Ellipse",
		wxBitmap("Icons/Ellipse.png", wxBITMAP_TYPE_PNG),
		"Draw Ellipse", wxITEM_CHECK);
	mToolbar->AddTool(ID_DrawRect, "Draw Rectangle",
		wxBitmap("Icons/Rectangle.png", wxBITMAP_TYPE_PNG),
		"Draw Rectangle", wxITEM_CHECK);
	mToolbar->AddTool(ID_DrawPencil, "Pencil",
		wxBitmap("Icons/Pencil.png", wxBITMAP_TYPE_PNG),
		"Pencil", wxITEM_CHECK);

	mToolbar->Realize();

	ToggleTool(ID_Selector);

	// Both undo and redo are disabled initially
	mToolbar->EnableTool(wxID_UNDO, false);
	mToolbar->EnableTool(wxID_REDO, false);
}

void PaintFrame::SetupModelAndView()
{
	// Prepare the draw panel and show this frame
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	mPanel = new PaintDrawPanel(this);
	sizer->Add(mPanel, 1, wxEXPAND);

	// Programatically bind the mouse events on the draw panel to us
	mPanel->Bind(wxEVT_LEFT_DOWN, &PaintFrame::OnMouseButton, this);
	mPanel->Bind(wxEVT_LEFT_UP, &PaintFrame::OnMouseButton, this);
	mPanel->Bind(wxEVT_MOTION, &PaintFrame::OnMouseMove, this);

	// Create the model
	mModel = std::make_shared<PaintModel>();
	mPanel->SetModel(mModel);
	SetSizer(sizer);

	SetAutoLayout(true);
}

void PaintFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void PaintFrame::OnNew(wxCommandEvent& event)
{
	mModel->New();
	mPanel->PaintNow();
    UpdateUndoRedo();
}

void PaintFrame::OnExport(wxCommandEvent& event)
{
	wxFileDialog 
        saveFileDialog(this, _("Save image file"), "", "",
                       "image files (*.png)|*.png|(*.bmp)|*.bmp|(*.jpeg)|*.jpeg", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    
    if (saveFileDialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }
    
    wxFileOutputStream output_stream(saveFileDialog.GetPath());
    if (!output_stream.IsOk())
    {
        wxLogError("Cannot save current contents in file '%s'.", saveFileDialog.GetPath());
        return;
    }
    else
    {
        mModel->ExportImage(saveFileDialog.GetPath(), saveFileDialog.GetFilename());
    }
}

void PaintFrame::OnImport(wxCommandEvent& event)
{
	wxFileDialog 
        openFileDialog(this, _("Open image file"), "", "",
                       "image files (*.png)|*.png|(*.bmp)|*.bmp|(*.jpeg)|*.jpeg", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    
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
        mModel->ImportImage(openFileDialog.GetPath(), openFileDialog.GetFilename());
        mPanel->PaintNow();
    }
}

void PaintFrame::OnUndo(wxCommandEvent& event)
{
	mModel->Undo();
    mPanel->PaintNow();
    UpdateUndoRedo();
}

void PaintFrame::OnRedo(wxCommandEvent& event)
{
	mModel->Redo();
    mPanel->PaintNow();
    UpdateUndoRedo();
}

void PaintFrame::UpdateUndoRedo()
{
    //Disable undo if no more commands
    if (!mModel->CanUndo())
    {
        mToolbar->EnableTool(wxID_UNDO, false);
        mEditMenu->Enable(wxID_UNDO, false);
    }
    else
    {
    //Enable Undo
        mToolbar->EnableTool(wxID_UNDO, true);
        mEditMenu->Enable(wxID_UNDO, true);
    }
    
    //Disable redo if no more commands
    if (!mModel->CanRedo())
    {
        mToolbar->EnableTool(wxID_REDO, false);
        mEditMenu->Enable(wxID_REDO, false);
    }
    else
    {
    //Enable Redo
        mToolbar->EnableTool(wxID_REDO, true);
        mEditMenu->Enable(wxID_REDO, true);
    }
}

void PaintFrame::OnUnselect(wxCommandEvent& event)
{
    //Call unselect command if there is currently one selected
	if (mModel->HasSelectedShape())
    {
        mModel->UnselectShape();
        mPanel->PaintNow();
    }
    mEditMenu->Enable(ID_Unselect, false);
    mEditMenu->Enable(ID_Delete, false);}

void PaintFrame::OnDelete(wxCommandEvent& event)
{
    if (mModel->HasSelectedShape())
    {
        wxPoint nullPoint;
        mModel->CreateCommand(CM_Delete, nullPoint);
        mModel->FinalizeCommand();
        mPanel->PaintNow();
    }
    mEditMenu->Enable(ID_Unselect, false);
    mEditMenu->Enable(ID_Delete, false);
    
    SetCursor(CU_Default);
}

void PaintFrame::OnSetPenColor(wxCommandEvent& event)
{
    //Display color options
	wxColourData data;
    data.SetColour(mModel->GetPenColor());
    
    //Either create setCommand if selected object exists or simply change pen color
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
    {
        if (mModel->HasSelectedShape())
        {
            mModel->SetSelectedPenColor(dialog.GetColourData().GetColour());
            mModel->SetSelectedPenSize(mModel->GetSelectedShape()->GetPen().GetWidth());

            wxPoint nullPoint;
            mModel->CreateCommand(CM_SetPen, nullPoint);
            mModel->FinalizeCommand();
            mPanel->PaintNow();
        }
        else
        {
            mModel->SetPenColor(dialog.GetColourData().GetColour());
        }
    }
}

void PaintFrame::OnSetPenWidth(wxCommandEvent& event)
{
	wxTextEntryDialog textData;
    textData.Create(GetParent(), "Enter in an integer from 1-10");
    
    if (textData.ShowModal() == wxID_OK)
    {
        int enteredValue = std::stoi(textData.GetValue().ToStdString());
        
        if (enteredValue >= 1 && enteredValue <= 10)
        {
            if (mModel->HasSelectedShape())
            {
                mModel->SetSelectedPenSize(enteredValue);
                mModel->SetSelectedPenColor(mModel->GetSelectedShape()->GetPen().GetColour());
                wxPoint nullPoint;
                mModel->CreateCommand(CM_SetPen, nullPoint);
                mModel->FinalizeCommand();
                mPanel->PaintNow();
            }

            mModel->SetPenWidth(enteredValue);
        }
    }
}

void PaintFrame::OnSetBrushColor(wxCommandEvent& event)
{
    //Display color options
	wxColourData data;
    data.SetColour(mModel->GetBrushColor());
    
    //Either create setCommand if selected object exists or simply change brush color
    wxColourDialog dialog(this, &data);
    if (dialog.ShowModal() == wxID_OK)
    {
        if (mModel->HasSelectedShape())
        {
            mModel->SetSelectedBrushColor(dialog.GetColourData().GetColour());
            
            wxPoint nullPoint;
            mModel->CreateCommand(CM_SetBrush, nullPoint);
            mModel->FinalizeCommand();
            mPanel->PaintNow();
        }
        else
        {
            mModel->SetBrushColor(dialog.GetColourData().GetColour());
        }
    }
}

void PaintFrame::OnMouseButton(wxMouseEvent& event)
{
	if (event.LeftDown())
	{
		if (mCurrentTool == ID_DrawRect)
        {
            mModel->CreateCommand(CM_DrawRect, event.GetPosition());
            mPanel->PaintNow();
        }
        if (mCurrentTool == ID_DrawEllipse)
        {
            mModel->CreateCommand(CM_DrawEllipse, event.GetPosition());
            mPanel->PaintNow();
        }
        if (mCurrentTool == ID_DrawLine)
        {
            mModel->CreateCommand(CM_DrawLine, event.GetPosition());
            mPanel->PaintNow();
        }
        if (mCurrentTool == ID_DrawPencil)
        {
            mModel->CreateCommand(CM_DrawPencil, event.GetPosition());
            mPanel->PaintNow();
        }
        if (mCurrentTool == ID_Selector)
        {
            //Unselect and delete menu options
            if (mModel->SetSelectedShape(event.GetPosition()))
            {
                mEditMenu->Enable(ID_Unselect, true);
                mEditMenu->Enable(ID_Delete, true);
            }
            else
            {
                mEditMenu->Enable(ID_Unselect, false);
                mEditMenu->Enable(ID_Delete, false);
            }
            
            //Move command
            if (mModel->HasSelectedShape())
            {
                if (mModel->GetSelectedShape()->Intersects(event.GetPosition()))
                {
                    mModel->CreateCommand(CM_Move, event.GetPosition());
                    mModel->UpdateCommand(event.GetPosition());
                }
            }
            mPanel->PaintNow();
        }
	}
	else if (event.LeftUp())
	{
        if (mModel->HasActiveCommand())
        {
            mModel->UpdateCommand(event.GetPosition());
            mModel->FinalizeCommand();
            mPanel->PaintNow();
            
            UpdateUndoRedo();
        }
	}
}

void PaintFrame::OnMouseMove(wxMouseEvent& event)
{
    if (mModel->HasActiveCommand())
    {
        mModel->UpdateCommand(event.GetPosition());
        mPanel->PaintNow();
    }
    
    //Change cursor for move
    if (mCurrentTool == ID_Selector && mModel->HasSelectedShape())
    {
        if (mModel->GetSelectedShape()->Intersects(event.GetPosition()))
        {
            SetCursor(CU_Move);
        }
        else
        {
            SetCursor(CU_Default);
        }
    }
}

void PaintFrame::ToggleTool(EventID toolID)
{
	// Deselect everything
	for (int i = ID_Selector; i <= ID_DrawPencil; i++)
	{
		mToolbar->ToggleTool(i, false);
	}

	// Select the new tool
	mToolbar->ToggleTool(toolID, true);

	mCurrentTool = toolID;
}

void PaintFrame::SetCursor(CursorType type)
{
	wxCursor* cursor = mCursors.GetCursor(type);
	if (cursor != nullptr)
	{
		mPanel->SetCursor(*cursor);
	}
}

void PaintFrame::OnSelectTool(wxCommandEvent& event)
{
	EventID id = static_cast<EventID>(event.GetId());
	ToggleTool(id);

	// Select appropriate cursor
	switch (id)
	{
	case ID_DrawLine:
	case ID_DrawEllipse:
	case ID_DrawRect:
		SetCursor(CU_Cross);
		break;
	case ID_DrawPencil:
		SetCursor(CU_Pencil);
		break;
	default:
		SetCursor(CU_Default);
		break;
	}
}
