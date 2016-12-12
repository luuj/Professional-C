#pragma once
#include <memory>
#include <vector>
#include <stack>
#include "Shape.h"
#include "Command.h"
#include <wx/bitmap.h>

class PaintModel : public std::enable_shared_from_this<PaintModel>
{
public:
	PaintModel();
	
	// Draws any shapes in the model to the provided DC (draw context)
	void DrawShapes(wxDC& dc, bool showSelection = true);

	// Clear the current paint model and start fresh
	void New();

	// Add a shape to the paint model
	void AddShape(std::shared_ptr<Shape> shape);
	// Remove a shape from the paint model
	void RemoveShape(std::shared_ptr<Shape> shape);

    //Command functions
    bool HasActiveCommand();
    void CreateCommand(CommandType type, wxPoint startPoint);
    void UpdateCommand(wxPoint point);
    void FinalizeCommand();
    bool CanUndo();
    bool CanRedo();
    void Undo();
    void Redo();
    
    //Pen and brush functions
    void SetPenWidth(int width);
    int GetPenWidth();
    void SetPenColor(wxColour color);
    wxColour GetPenColor();
    void SetBrushColor(wxColour color);
    wxColour GetBrushColor();
    wxPen GetPen();
    wxBrush GetBrush();
    void ResetColors();
    
    //Set Pen/brush functions
    void SetSelectedPenColor(wxColour col);
    wxPen GetSelectedPen();
    void SetSelectedBrushColor(wxColour col);
    wxColour GetSelectedBrushColor();
    void SetSelectedPenSize(int size);
    
    //Selection functions
    bool SetSelectedShape(wxPoint point);
    bool HasSelectedShape();
    std::shared_ptr<Shape> GetSelectedShape();
    void UnselectShape();
    
    //Export/Import functions
    void ExportImage(wxString fileName, wxString extension);
    void ImportImage(wxString filePath, wxString extension);
    
private:
	// Vector of all the shapes in the model
	std::vector<std::shared_ptr<Shape>> mShapes;
    std::shared_ptr<Command> activeCommand;
    std::shared_ptr<Shape> selectedShape;

    //Undo and redo stacks
    std::stack<std::shared_ptr<Command>> undoStack;
    std::stack<std::shared_ptr<Command>> redoStack;

    //Current model pen/brush and selected pen/brush
    wxPen currentPen;
    wxBrush currentBrush;
    wxPen selectedPen;
    wxBrush selectedBrush;
    
    wxBitmap importedImage;
};
