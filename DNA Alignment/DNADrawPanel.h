//
//  DNADrawPanel.h
//  DNAwx
//
//  Created by Sanjay Madhav on 12/27/14.
//  Copyright (c) 2014 Sanjay Madhav. All rights reserved.
//

#pragma once
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <string>
#include <memory>
#include <vector>

class DNADrawPanel : public wxPanel
{
public:
	DNADrawPanel(wxFrame* parent);
	void PaintNow();
    void RetrieveData(const std::string& header, const std::vector<int>& aminoCount);
    void New();

protected:
	void PaintEvent(wxPaintEvent & evt);
	void Render(wxDC& dc);
    void CreateAminoPercentages();
    void CreateSideText(wxDC& dc);
 
	DECLARE_EVENT_TABLE()
	
private:
    std::string mHeader;
    std::vector<int> mAminoCount;
    std::vector<double> mAminoPercentages;
    std::vector<double> mBarPercentages;
};

