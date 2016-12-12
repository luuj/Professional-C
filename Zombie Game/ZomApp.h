#pragma once
#include <wx/app.h>

class ZomApp : public wxApp
{
public:
	virtual bool OnInit();
private:
	class ZomFrame* mFrame;
};
