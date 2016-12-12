//
//  DNADrawPanel.cpp
//  DNAwx
//
//  Created by Sanjay Madhav on 12/27/14.
//  Copyright (c) 2014 Sanjay Madhav. All rights reserved.
//

#include "DNADrawPanel.h"
#include <wx/dcclient.h>
#include <wx/sizer.h>
#include <array>
#include <sstream>
#include <iomanip>
#include <iostream>

BEGIN_EVENT_TABLE(DNADrawPanel, wxPanel)
	EVT_PAINT(DNADrawPanel::PaintEvent)
END_EVENT_TABLE()

DNADrawPanel::DNADrawPanel(wxFrame* parent)
: wxPanel(parent)
{
	
}

void DNADrawPanel::PaintEvent(wxPaintEvent & evt)
{
	wxPaintDC dc(this);
	Render(dc);
}

void DNADrawPanel::PaintNow()
{
	wxClientDC dc(this);
	Render(dc);
}

void DNADrawPanel::Render(wxDC& dc)
{
	// Clear
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();
	
    //Header
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawText(mHeader, 30, 30);
    
    //Draw histogram when data is loaded
    if (mHeader != "")
    {
        CreateAminoPercentages();
        CreateSideText(dc);
    }

}

void DNADrawPanel::RetrieveData(const std::string &header, const std::vector<int> &aminoCount)
{
    mHeader = header;
    mAminoCount = aminoCount;
}

void DNADrawPanel::CreateAminoPercentages()
{
    int totalCount = 0;
    int maxCount = 0;
    
    //Find total # of amino acids created
    for (int i=0; i<mAminoCount.size(); i++)
    {
        totalCount += mAminoCount.at(i);
        maxCount = (mAminoCount.at(i) > maxCount) ? mAminoCount.at(i) : maxCount;
    }
    
    double aminoPercentage, barPercentage;
    //Store percentages in member variables
    for (int i=0; i<mAminoCount.size(); i++)
    {
        aminoPercentage = (static_cast<double>(mAminoCount[i]) / static_cast<double>(totalCount)) * 100;
        barPercentage = (static_cast<double>(mAminoCount[i]) / static_cast<double>(maxCount));
        
        if (totalCount == 0)
        {
            aminoPercentage = 0;
            barPercentage = 0;
        }
        mAminoPercentages.push_back(aminoPercentage);
        mBarPercentages.push_back(barPercentage);
    }    
}

void DNADrawPanel::CreateSideText(wxDC& dc)
{
    std::vector<std::string> aminoNames = {"Alanine", "Arginine", "Asparagine",
        "Aspartic Acid", "Cysteine", "Glutamine", "Glutamic Acid", "Glycine",
        "Histidine", "Isoleucine", "Methionine", "Leucine", "Lysine", "Phenylalanine",
        "Proline", "Serine", "Threonine", "Tryptophan", "Tyrosine", "Valine"};
    std::ostringstream oss;
    std::vector<wxColour> colorRotation = {*wxRED, *wxGREEN, *wxBLUE};
    std::vector<wxBrush> brushRotation = {*wxRED_BRUSH, *wxGREEN_BRUSH, *wxBLUE_BRUSH};
    
    for (int i=0; i<20; i++)
    {
        oss.str("");
        oss.clear();
        
        //Side Text
        dc.SetTextForeground(colorRotation[i%3]);
        oss << aminoNames[i] <<": " << mAminoPercentages[i] << "% (" << mAminoCount[i] << ")";
        dc.DrawText(oss.str(), 30, 70+(i*30));
        
        
        //Rectangle bars
        dc.SetBrush(brushRotation[i%3]);
        dc.DrawRectangle(250, 70+(i*30), (720*mBarPercentages[i]), 20);
    }
    
}

void DNADrawPanel::New()
{
    mHeader = "";
    mBarPercentages.clear();
    mAminoCount.clear();
    mAminoPercentages.clear();
}
