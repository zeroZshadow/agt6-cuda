#include "GUI.h"
#include "Source\Render\Defines.h"

extern void updateProgram(GenerateInfo agInfo);
extern void UpdateGenInfo(GenerateInfo agInfo);


System::Void CudaTest::GUI::Update(System::Object^  sender, System::EventArgs^  e) 
{
	GenerateInfo genInfo;
	if (comboBox1->Text == "Floor")	{
		genInfo.genType = GM_FLOOR;
	} else if (comboBox1->Text == "Sphere")	{
		genInfo.genType = GM_SPHERE;
	} else {
		genInfo.genType = GM_CAVES;
	}

	genInfo.floorWeight = QUICK_RANGE_MAP( (float)FloorBar->Value, 0.f, 100.f, FLOOR_GEN_RANGE_MIN, FLOOR_GEN_RANGE_MAX) ;
	genInfo.sphereRad = QUICK_RANGE_MAP( (float)SphereBar->Value, 0.f, 100.f, FLOOR_GEN_RANGE_MIN, FLOOR_GEN_RANGE_MAX) ;
	genInfo.prlnWeight1 = QUICK_RANGE_MAP( (float)WeightBar1->Value, 0.f, 100.f, PERLIN_WEIGHT_RANGE_MIN, PERLIN_WEIGHT_RANGE_MAX) ;
	genInfo.prlnWeight2 = QUICK_RANGE_MAP( (float)WeightBar2->Value, 0.f, 100.f, PERLIN_WEIGHT_RANGE_MIN, PERLIN_WEIGHT_RANGE_MAX) ;
	genInfo.prlnWeight3 = QUICK_RANGE_MAP( (float)WeightBar3->Value, 0.f, 100.f, PERLIN_WEIGHT_RANGE_MIN, PERLIN_WEIGHT_RANGE_MAX) ;
	genInfo.prlnWeight4 = 0;//QUICK_RANGE_MAP( (float)FloorBar->Value, 0.f, 100.f, FLOOR_GEN_RANGE_MIN, FLOOR_GEN_RANGE_MAX) ;

	genInfo.prlnNoise1 = QUICK_RANGE_MAP( (float)NoiseBar1->Maximum - (float)NoiseBar1->Value, 0.f, (float)NoiseBar1->Maximum, PERLIN_NOISE_RANGE_MIN, PERLIN_NOISE_RANGE_MAX) ;
	genInfo.prlnNoise2 = QUICK_RANGE_MAP( (float)NoiseBar2->Maximum - (float)NoiseBar2->Value, 0.f, (float)NoiseBar2->Maximum, PERLIN_NOISE_RANGE_MIN, PERLIN_NOISE_RANGE_MAX) ;
	genInfo.prlnNoise3 = QUICK_RANGE_MAP( (float)NoiseBar3->Maximum - (float)NoiseBar3->Value, 0.f, (float)NoiseBar3->Maximum, PERLIN_NOISE_RANGE_MIN, PERLIN_NOISE_RANGE_MAX) ;
	genInfo.prlnNoise4 = 0;//QUICK_RANGE_MAP( (float)FloorBar->Value, 0.f, 100.f, FLOOR_GEN_RANGE_MIN, FLOOR_GEN_RANGE_MAX) ;

	 genInfo.iso = QUICK_RANGE_MAP( (float)IsoValue->Value, 0.f, (float)IsoValue->Maximum, -20.0f, 20.0f) ;

	if(UpdateCheckbox->Checked)
	{
		UpdateGenInfo(genInfo);
	}

	updateProgram(genInfo);
}

