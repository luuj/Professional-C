#include "Shape.h"

Shape::Shape(const wxPoint& start)
	:mStartPoint(start)
	,mEndPoint(start)
	,mTopLeft(start)
	,mBotRight(start)
    ,offset(0,0)
{

}

// Tests whether the provided point intersects
// with this shape
bool Shape::Intersects(const wxPoint& point) const
{
	wxPoint topleft;
	wxPoint botright;
	GetBounds(topleft, botright);
	if (point.x >= topleft.x && point.x <= botright.x &&
		point.y >= topleft.y && point.y <= botright.y)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Update shape with new provided point
void Shape::Update(const wxPoint& newPoint)
{
	mEndPoint = newPoint;

	// For most shapes, we only have two points - start and end
	// So we can figure out the top left/bottom right bounds
	// based on this.
	mTopLeft.x = std::min(mStartPoint.x, mEndPoint.x);
	mTopLeft.y = std::min(mStartPoint.y, mEndPoint.y);
	mBotRight.x = std::max(mStartPoint.x, mEndPoint.x);
	mBotRight.y = std::max(mStartPoint.y, mEndPoint.y);
}

void Shape::Finalize()
{
	// Default finalize doesn't do anything
}

void Shape::GetBounds(wxPoint& topLeft, wxPoint& botRight) const
{
	topLeft = mTopLeft + offset;
	botRight = mBotRight + offset;
}

void Shape::SetPen(wxPen pen)
{
    currentPen = pen;
}

void Shape::SetBrush(wxBrush brush)
{
    currentBrush = brush;
}

wxBrush Shape::GetBrush()
{
    return currentBrush;
}

wxPen Shape::GetPen()
{
    return currentPen;
}

void Shape::DrawSelection(wxDC& dc)
{
    wxPen dottedPen = *wxBLACK_DASHED_PEN;
    wxBrush trBrush = *wxTRANSPARENT_BRUSH;
    
    wxPoint biggerTopLeft;
    biggerTopLeft.x = mTopLeft.x-2+offset.x;
    biggerTopLeft.y = mTopLeft.y-2+offset.y;
    
    wxPoint biggerBottomRight;
    biggerBottomRight.x = mBotRight.x+2+offset.x;
    biggerBottomRight.y = mBotRight.y+2+offset.y;
    
    dc.SetPen(dottedPen);
    dc.SetBrush(trBrush);
    wxRect rect = *new wxRect(biggerTopLeft, biggerBottomRight);
    dc.DrawRectangle(rect);
}

//Rectangle Shape
RectShape::RectShape(const wxPoint& start) : Shape(start){}

void RectShape::Draw(wxDC &dc) const
{
    dc.SetPen(currentPen);
    dc.SetBrush(currentBrush);
    wxRect rect = *new wxRect(mTopLeft+offset, mBotRight+offset);
    dc.DrawRectangle(rect);
}

//Ellipse Shape
EllipseShape::EllipseShape(const wxPoint& start) : Shape(start){}

void EllipseShape::Draw(wxDC &dc) const
{
    dc.SetPen(currentPen);
    dc.SetBrush(currentBrush);
    wxRect rect = *new wxRect(mTopLeft+offset, mBotRight+offset);
    dc.DrawEllipse(rect);
}

//Line Shape
LineShape::LineShape(const wxPoint& start) : Shape(start){}

void LineShape::Draw(wxDC &dc) const
{
    dc.SetPen(currentPen);
    dc.SetBrush(currentBrush);
    dc.DrawLine(mStartPoint+offset, mEndPoint+offset);
}

//Pencil Shape
PencilShape::PencilShape(const wxPoint& start) : Shape(start)
{
    pencilPoints.push_back(start);
}

void PencilShape::Draw(wxDC &dc) const
{
    dc.SetPen(currentPen);
    dc.SetBrush(currentBrush);
    if (pencilPoints.size() == 1)
    {
        dc.DrawPoint(pencilPoints.at(0) + offset);
    }
    else
    {
        int vectorSize = static_cast<int>(pencilPoints.size());
        dc.DrawLines(vectorSize, pencilPoints.data(), offset.x, offset.y);
    }
}

void PencilShape::Update(const wxPoint &newPoint)
{
    Shape::Update(newPoint);
    pencilPoints.push_back(newPoint);
}

void PencilShape::Finalize()
{
    if (pencilPoints.size() == 0)
        return;
    
    wxPoint firstPoint = pencilPoints.at(0);
    
    int leftMost = firstPoint.x, topMost = firstPoint.y;
    int bottomMost = firstPoint.y, rightMost = firstPoint.x;
    
    for (wxPoint point : pencilPoints)
    {
        if (point.x < leftMost)
            leftMost = point.x;
        if (point.x > rightMost)
            rightMost = point.x;
        if (point.y < topMost)
            topMost = point.y;
        if (point.y > bottomMost)
            bottomMost = point.y;
    }
    
    mTopLeft.x = leftMost;
    mTopLeft.y = topMost;
    mBotRight.x = rightMost;
    mBotRight.y = bottomMost;
}

//Book-keeping for pen undo/redo
void Shape::SetOldPen()
{
    oldPen = currentPen;
}

wxPen Shape::GetOldPen()
{
    return oldPen;
}

void Shape::SetOldBrush()
{
    oldBrush = currentBrush;
}

wxBrush Shape::GetOldBrush()
{
    return oldBrush;
}

wxPoint Shape::GetTopLeft()
{
    return mTopLeft;
}

void Shape::SetOffset(wxPoint point)
{
    offset = point;
}

wxPoint Shape::GetOffset()
{
    return offset;
}
