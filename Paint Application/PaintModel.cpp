#include "PaintModel.h"
#include <algorithm>
#include <wx/dcmemory.h>
#include <string>

PaintModel::PaintModel()
{
	ResetColors();
}

// Draws any shapes in the model to the provided DC (draw context)
void PaintModel::DrawShapes(wxDC& dc, bool showSelection)
{
    //If imported image exists, draw that first
    if (importedImage.IsOk())
    {
        dc.DrawBitmap(importedImage, 0, 0);
    }
    
	for (std::shared_ptr<Shape> shape : mShapes)
    {
        shape->Draw(dc);
    }
    
    //If there is a selected shape, draw the lines around it
    if (HasSelectedShape())
        selectedShape->DrawSelection(dc);
}

// Clear the current paint model and start fresh
void PaintModel::New()
{
	mShapes.clear();
    activeCommand.reset();
    
    //Clear undo/redo stacks
    while (undoStack.size() != 0)
    {
        undoStack.pop();
    }
    while (redoStack.size() != 0)
    {
        redoStack.pop();
    }
    
    //Set brush and pen type to default
    ResetColors();
    selectedShape.reset();
    
    if (importedImage.IsOk())
    {
        wxBitmap newMap;
        importedImage = newMap;
    }
}

// Add a shape to the paint model
void PaintModel::AddShape(std::shared_ptr<Shape> shape)
{
	mShapes.emplace_back(shape);
}

// Remove a shape from the paint model
void PaintModel::RemoveShape(std::shared_ptr<Shape> shape)
{
    //Check for selection upon undo
    if (shape == selectedShape)
        selectedShape.reset();
    
	auto iter = std::find(mShapes.begin(), mShapes.end(), shape);
	if (iter != mShapes.end())
	{
		mShapes.erase(iter);
	}
}

/****Command Functions*****/
bool PaintModel::HasActiveCommand()
{
    return (activeCommand != nullptr);
}

void PaintModel::CreateCommand(CommandType type, wxPoint startPoint)
{
    activeCommand = CommandFactory::Create(shared_from_this(), type, startPoint);
}

void PaintModel::UpdateCommand(wxPoint point)
{
    activeCommand->Update(point);
}

void PaintModel::FinalizeCommand()
{
    activeCommand->Finalize(shared_from_this());
    
    //Undo/redo stack updates
    undoStack.push(activeCommand);
    while (redoStack.size() != 0)
    {
        redoStack.pop();
    }
    activeCommand.reset();
}

bool PaintModel::CanRedo()
{
    return (redoStack.size() == 0) ? false : true;
}

bool PaintModel::CanUndo()
{
    return (undoStack.size() == 0) ? false : true;
}

void PaintModel::Undo()
{
    std::shared_ptr<Command> undoneCommand = undoStack.top();
    undoStack.pop();
    undoneCommand->Undo(shared_from_this());
        
    redoStack.push(undoneCommand);
}

void PaintModel::Redo()
{
    std::shared_ptr<Command> redoneCommand = redoStack.top();
    redoStack.pop();
    redoneCommand->Redo(shared_from_this());
    
    undoStack.push(redoneCommand);
}

/*****Pen/Brush Functions*****/
void PaintModel::SetPenColor(wxColour color)
{
    currentPen.SetColour(color);
}

void PaintModel::SetPenWidth(int width)
{
    currentPen.SetWidth(width);
}

void PaintModel::SetBrushColor(wxColour color)
{
    currentBrush.SetColour(color);
}

int PaintModel::GetPenWidth()
{
    return currentPen.GetWidth();
}

wxColour PaintModel::GetPenColor()
{
    return currentPen.GetColour();
}

wxColour PaintModel::GetBrushColor()
{
    return currentBrush.GetColour();
}

wxPen PaintModel::GetPen()
{
    return currentPen;
}

wxBrush PaintModel::GetBrush()
{
    return  currentBrush;
}

void PaintModel::ResetColors()
{
    currentPen = *wxBLACK_PEN;
    currentBrush = *wxWHITE_BRUSH;
    
    selectedPen = currentPen;
    selectedBrush = currentBrush;
}

/*****Selection functions*****/
bool PaintModel::SetSelectedShape(wxPoint point)
{
    int shapeNum = static_cast<int>(mShapes.size())-1;
    std::shared_ptr<Shape> currShape;
    for (int i=shapeNum; i>=0; i--)
    {
        currShape = mShapes.at(i);
        if (currShape->Intersects(point))
        {
            selectedShape = currShape;
            return true;;
        }
    }
    
    //If no shape is selected, reset it just in case
    selectedShape.reset();
    return false;
}

bool PaintModel::HasSelectedShape()
{
    return (selectedShape != nullptr) ? true : false;
}

std::shared_ptr<Shape> PaintModel::GetSelectedShape()
{
    return selectedShape;
}

void PaintModel::SetSelectedPenColor(wxColour col)
{
    selectedPen.SetColour(col);
}

wxPen PaintModel::GetSelectedPen()
{
    return selectedPen;
}

void PaintModel::SetSelectedBrushColor(wxColour col)
{
    selectedBrush.SetColour(col);
}

wxColour PaintModel::GetSelectedBrushColor()
{
    return selectedBrush.GetColour();
}

void PaintModel::SetSelectedPenSize(int size)
{
    selectedPen.SetWidth(size);
}

void PaintModel::UnselectShape()
{
    selectedShape.reset();
}

/*****Export/Import functions*****/
void PaintModel::ExportImage(wxString fileName, wxString extension)
{
    wxBitmap bitmap;
    bitmap.Create(wxSize(1024, 768));
    
    wxMemoryDC dc(bitmap);
    dc.SetBackground(*wxWHITE_BRUSH);
    dc.Clear();
    
    UnselectShape();
    DrawShapes(dc);
    
    wxBitmapType type = wxBITMAP_TYPE_PNG;

    std::string fileType = extension.ToStdString();
    if (fileType.substr(fileType.find_last_of(".") + 1) == "jpeg")
        type = wxBITMAP_TYPE_JPEG;
    if (fileType.substr(fileType.find_last_of(".") + 1) == "bmp")
        type = wxBITMAP_TYPE_BMP;
    
    bitmap.SaveFile(fileName, type);

}

void PaintModel::ImportImage(wxString filePath, wxString extension)
{
    New();
    
    wxBitmapType type = wxBITMAP_TYPE_PNG;

    std::string fileType = extension.ToStdString();
    if (fileType.substr(fileType.find_last_of(".") + 1) == "jpeg")
        type = wxBITMAP_TYPE_JPEG;
    if (fileType.substr(fileType.find_last_of(".") + 1) == "bmp")
        type = wxBITMAP_TYPE_BMP;
    
    importedImage.LoadFile(filePath, type);
}


