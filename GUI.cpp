#include "GUI.h"

extern void updateProgram();

System::Void CudaTest::GUI::Update(System::Object^  sender, System::EventArgs^  e) 
			 {
				updateProgram();
			 }