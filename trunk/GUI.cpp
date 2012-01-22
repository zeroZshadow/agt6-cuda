#include "GUI.h"
#include "Source\Render\Defines.h"

extern void updateProgram();


System::Void CudaTest::GUI::Update(System::Object^  sender, System::EventArgs^  e) 
{
	updateProgram();
}

