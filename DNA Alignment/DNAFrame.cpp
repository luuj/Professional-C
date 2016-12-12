//
//  DNAFrame.cpp
//  DNAwx
//
//  Created by Sanjay Madhav on 12/27/14.
//  Copyright (c) 2014 Sanjay Madhav. All rights reserved.
//

#include "DNAFrame.h"
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/filedlg.h>
#include <wx/busyinfo.h>
#include <wx/wfstream.h>
#include "DNADrawPanel.h"
#include "Exceptions.h"
#include "DNAAlignDlg.h"
#include "FASTAParser.h"
#include "DNATranslator.h"
#include "PairwiseOperator.h"

enum
{
	ID_AMINO_HIST=1000,
    ID_PAIRWISE_HIST=2000
};

wxBEGIN_EVENT_TABLE(DNAFrame, wxFrame)
	EVT_MENU(wxID_EXIT, DNAFrame::OnExit)
	EVT_MENU(wxID_NEW, DNAFrame::OnNew)
	EVT_MENU(ID_AMINO_HIST, DNAFrame::OnAminoHist)
    EVT_MENU(ID_PAIRWISE_HIST, DNAFrame::OnPairwise)
wxEND_EVENT_TABLE()

DNAFrame::DNAFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	// File menu
	wxMenu* menuFile = new wxMenu;
	menuFile->Append(wxID_NEW);
	menuFile->Append(ID_AMINO_HIST, "Amino Acid Histogram...",
					 "Create a histogram from a FASTA file.");
    menuFile->Append(ID_PAIRWISE_HIST, "Pairwise Alignment...",
                     "Create a pairwise alignment comparing two FASTA files.");
	menuFile->Append(wxID_EXIT);
	
	wxMenuBar* menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	SetMenuBar(menuBar);
	CreateStatusBar();
	
	// Prepare the draw panel and show this frame
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	mPanel = new DNADrawPanel(this);
	sizer->Add(mPanel, 1, wxEXPAND);
	
	SetSizer(sizer);
	SetAutoLayout(true);
	Show(true);
	
	SetMinSize(GetSize());
	SetMaxSize(GetSize());
}

void DNAFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void DNAFrame::OnNew(wxCommandEvent& event)
{
    mPanel->New();
    mPanel->PaintNow();
}

void DNAFrame::OnAminoHist(wxCommandEvent& event)
{
    wxFileDialog 
        openFileDialog(this, _("Open FASTA file"), "./data", "",
                       "FASTA file (*.fasta)|*.fasta", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
    
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
            //Clear existing histogram
            mPanel->New();
            mPanel->PaintNow();
            
            FASTAParser fp = *new FASTAParser(openFileDialog.GetPath().ToStdString());
            DNATranslator dt = *new DNATranslator(fp.GetSequence());

            //Transfer calculated data to panel to draw
            mPanel->RetrieveData(fp.GetHeader(), dt.GetAminoCount());
            mPanel->PaintNow();
        }
        catch(FileLoadExcept ex)
        {
            wxMessageBox("FASTA file is invalid", "Error", wxOK | wxICON_ERROR);
        }
    }

}

void DNAFrame::OnPairwise(wxCommandEvent &event)
{
    DNAAlignDlg pairwiseWindow;
    if (pairwiseWindow.ShowModal() == wxID_OK)
    {
        //Create new class to run algorithm
        try
        {
            FASTAParser inputA = *new FASTAParser(pairwiseWindow.GetInputAPath());
            FASTAParser inputB = *new FASTAParser(pairwiseWindow.GetInputBPath());
            std::string outputName = pairwiseWindow.GetOutputPath();
            
            PairwiseOperator po = *new PairwiseOperator(inputA, inputB, outputName);
            po.RunNeedleman();
            	
        }
        catch(FileLoadExcept ex)
        {
            wxMessageBox("One of the FASTA files is invalid", "Error", wxOK | wxICON_ERROR);
        }
        
    }
}
