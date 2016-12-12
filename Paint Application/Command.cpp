#include "Command.h"
#include "Shape.h"
#include "PaintModel.h"

Command::Command(const wxPoint& start, std::shared_ptr<Shape> shape)
	:mStartPoint(start)
	,mEndPoint(start)
	,mShape(shape)
{

}

// Called when the command is still updating (such as in the process of drawing)
void Command::Update(const wxPoint& newPoint)
{
	mEndPoint = newPoint;
}

std::shared_ptr<Command> CommandFactory::Create(std::shared_ptr<PaintModel> model,
	CommandType type, const wxPoint& start)
{
	std::shared_ptr<Command> retVal;
    
    //Choose what to set return value to based on command type
    switch (type)
    {
        case CM_DrawRect:
        {
            std::shared_ptr<Shape> shapeVal = std::make_shared<RectShape>(start);
            shapeVal->SetPen(model->GetPen());
            shapeVal->SetBrush(model->GetBrush());
            
            retVal = std::make_shared<DrawCommand>(start, shapeVal);
            
            model->AddShape(shapeVal);
            break;
        }
        case CM_DrawLine:
        {
            std::shared_ptr<Shape> shapeVal = std::make_shared<LineShape>(start);
            shapeVal->SetPen(model->GetPen());
            shapeVal->SetBrush(model->GetBrush());
            
            retVal = std::make_shared<DrawCommand>(start, shapeVal);
            
            model->AddShape(shapeVal);
            break;
        }
        case CM_DrawEllipse:
        {
            std::shared_ptr<Shape> shapeVal = std::make_shared<EllipseShape>(start);
            shapeVal->SetPen(model->GetPen());
            shapeVal->SetBrush(model->GetBrush());
            
            retVal = std::make_shared<DrawCommand>(start, shapeVal);
            
            model->AddShape(shapeVal);
            break;
        }
        case CM_DrawPencil:
        {
            std::shared_ptr<Shape> shapeVal = std::make_shared<PencilShape>(start);
            shapeVal->SetPen(model->GetPen());
            shapeVal->SetBrush(model->GetBrush());
            
            retVal = std::make_shared<DrawCommand>(start, shapeVal);
            
            model->AddShape(shapeVal);
            break;
        }
        case CM_Move:
        {
            std::shared_ptr<Shape> movedShape = model->GetSelectedShape();
            retVal = std::make_shared<MoveCommand>(start, movedShape);
            break;
        }
        case CM_Delete:
        {
            std::shared_ptr<Shape> deletedShape = model->GetSelectedShape();
            model->RemoveShape(deletedShape);
            retVal = std::make_shared<DeleteCommand>(start, deletedShape);

            break;
        }
        case CM_SetPen:
        {
            std::shared_ptr<Shape> coloredShape = model->GetSelectedShape();
            coloredShape->SetOldPen();
            coloredShape->SetPen(model->GetSelectedPen());
            
            retVal = std::make_shared<SetPenCommand>(start, coloredShape);
            break;
        }
        case CM_SetBrush:
        {
            std::shared_ptr<Shape> coloredShape = model->GetSelectedShape();
            coloredShape->SetOldBrush();
            coloredShape->SetBrush(model->GetSelectedBrushColor());
            
            retVal = std::make_shared<SetBrushCommand>(start, coloredShape);
            break;
        }
    }

	return retVal;
}

/*****Draw Command*****/
DrawCommand::DrawCommand(const wxPoint& start, std::shared_ptr<Shape> shape) : Command(start, shape){}

void DrawCommand::Update(const wxPoint &newPoint)
{
    Command::Update(newPoint);
    mShape->Update(newPoint);
}

void DrawCommand::Finalize(std::shared_ptr<PaintModel> model)
{
    mShape->Finalize();
}

void DrawCommand::Undo(std::shared_ptr<PaintModel> model)
{
    model->RemoveShape(mShape);
}

void DrawCommand::Redo(std::shared_ptr<PaintModel> model)
{
    model->AddShape(mShape);
}

/*****Set Pen Command*****/
SetPenCommand::SetPenCommand(const wxPoint& start, std::shared_ptr<Shape> shape) : Command (start, shape)
{
    oldPen = mShape->GetOldPen();
    newPen = mShape->GetPen();
}

void SetPenCommand::Finalize(std::shared_ptr<PaintModel> model)
{
    mShape->Finalize();
}

void SetPenCommand::Undo(std::shared_ptr<PaintModel> model)
{
    mShape->SetPen(oldPen);
}

void SetPenCommand::Redo(std::shared_ptr<PaintModel> model)
{
    mShape->SetPen(newPen);
}

/*****Set Brush Command*****/
SetBrushCommand::SetBrushCommand(const wxPoint& start, std::shared_ptr<Shape> shape) : Command (start, shape)
{
    oldBrush = mShape->GetOldBrush();
    newBrush = mShape->GetBrush();
}

void SetBrushCommand::Finalize(std::shared_ptr<PaintModel> model)
{
    mShape->Finalize();
}

void SetBrushCommand::Undo(std::shared_ptr<PaintModel> model)
{
    mShape->SetBrush(oldBrush);
}

void SetBrushCommand::Redo(std::shared_ptr<PaintModel> model)
{
    mShape->SetBrush(newBrush);
}

/*****Delete Command*****/
DeleteCommand::DeleteCommand(const wxPoint& start, std::shared_ptr<Shape> shape) : Command(start, shape){}

void DeleteCommand::Finalize(std::shared_ptr<PaintModel> model)
{
    mShape->Finalize();
}

void DeleteCommand::Undo(std::shared_ptr<PaintModel> model)
{
    model->AddShape(mShape);
}

void DeleteCommand::Redo(std::shared_ptr<PaintModel> model)
{
    model->RemoveShape(mShape);
}

/*****Move Command*****/
MoveCommand::MoveCommand(const wxPoint& start, std::shared_ptr<Shape> shape) : Command(start, shape)
{
    initialPoint = start;
    oldOffset = shape->GetOffset();
}

void MoveCommand::Update(const wxPoint &newPoint)
{

    adjustedPoint.x = newPoint.x - initialPoint.x + oldOffset.x;
    adjustedPoint.y = newPoint.y - initialPoint.y + oldOffset.y;
    
    mShape->SetOffset(adjustedPoint);
}

void MoveCommand::Finalize(std::shared_ptr<PaintModel> model)
{
    mShape->Finalize();
}

void MoveCommand::Undo(std::shared_ptr<PaintModel> model)
{
    mShape->SetOffset(oldOffset);
}

void MoveCommand::Redo(std::shared_ptr<PaintModel> model)
{
    mShape->SetOffset(adjustedPoint);
}
