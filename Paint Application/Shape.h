#pragma once
#include <wx/dc.h>
#include <vector>

// Abstract base class for all Shapes
class Shape
{
public:
	Shape(const wxPoint& start);
	// Tests whether the provided point intersects
	// with this shape
	bool Intersects(const wxPoint& point) const;
	// Update shape with new provided point
	virtual void Update(const wxPoint& newPoint);
	// Finalize the shape -- when the user has finished drawing the shape
	virtual void Finalize();
	// Returns the top left/bottom right points of the shape
	void GetBounds(wxPoint& topLeft, wxPoint& botRight) const;
	// Draw the shape
	virtual void Draw(wxDC& dc) const = 0;
	virtual ~Shape() { }
    
    void SetPen(wxPen pen);
    void SetBrush(wxBrush brush);
    wxPen GetPen();
    wxBrush GetBrush();
    
    void SetOldPen();
    wxPen GetOldPen();
    void SetOldBrush();
    wxBrush GetOldBrush();
    
    void DrawSelection(wxDC& dc);
    wxPoint GetTopLeft();
    void SetOffset(wxPoint point);
    wxPoint GetOffset();
protected:
	// Starting point of shape
	wxPoint mStartPoint;
	// Ending point of shape
	wxPoint mEndPoint;
	// Top left point of shape
	wxPoint mTopLeft;
	// Bottom right point of shape
	wxPoint mBotRight;
    
    //Brushes and pens
    wxBrush currentBrush;
    wxPen currentPen;
    
    wxPen oldPen;
    wxBrush oldBrush;
    
    wxPoint offset;
};

class RectShape : public Shape
{
public:
    RectShape(const wxPoint& start);
    void Draw(wxDC& dc) const;
};

class EllipseShape : public Shape
{
    public:
        EllipseShape(const wxPoint& start);
        void Draw(wxDC& dc) const;
};

class LineShape : public Shape
{
    public:
        LineShape(const wxPoint& start);
        void Draw(wxDC& dc) const;
};

class PencilShape : public Shape
{
    public:
        PencilShape(const wxPoint& start);
        void Draw(wxDC& dc) const;
        void Update(const wxPoint& newPoint);
        void Finalize();
    protected:
        std::vector<wxPoint> pencilPoints;
};
