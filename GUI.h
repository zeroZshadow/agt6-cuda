#include "Source\Render\Defines.h"
#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

extern void generateTerrain(GenerateInfo agInfo);

namespace CudaTest {

	/// <summary>
	/// Summary for GUI
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class GUI : public System::Windows::Forms::Form
	{
	public:
		GUI(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			
		}


	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GUI()
		{
			if (components)
			{
				delete components;
			}
		}

	protected: 

	private: System::Windows::Forms::Timer^  Ticker;
	private: System::Windows::Forms::Button^  button1;

	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TrackBar^  FloorBar;


	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TrackBar^  SphereBar;

	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;

	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TrackBar^  NoiseBar3;

	private: System::Windows::Forms::TrackBar^  NoiseBar2;

	private: System::Windows::Forms::TrackBar^  NoiseBar1;

	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::Label^  label10;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::TrackBar^  WeightBar3;

	private: System::Windows::Forms::TrackBar^  WeightBar2;

	private: System::Windows::Forms::TrackBar^  WeightBar1;
	private: System::Windows::Forms::TrackBar^  IsoValue;

	private: System::Windows::Forms::Label^  label11;
	private: System::Windows::Forms::CheckBox^  UpdateCheckbox;






	private: System::ComponentModel::IContainer^  components;

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			this->Ticker = (gcnew System::Windows::Forms::Timer(this->components));
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->FloorBar = (gcnew System::Windows::Forms::TrackBar());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->SphereBar = (gcnew System::Windows::Forms::TrackBar());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->WeightBar3 = (gcnew System::Windows::Forms::TrackBar());
			this->WeightBar2 = (gcnew System::Windows::Forms::TrackBar());
			this->NoiseBar3 = (gcnew System::Windows::Forms::TrackBar());
			this->WeightBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->NoiseBar2 = (gcnew System::Windows::Forms::TrackBar());
			this->NoiseBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->IsoValue = (gcnew System::Windows::Forms::TrackBar());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->UpdateCheckbox = (gcnew System::Windows::Forms::CheckBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->FloorBar))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SphereBar))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->WeightBar3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->WeightBar2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NoiseBar3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->WeightBar1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NoiseBar2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NoiseBar1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->IsoValue))->BeginInit();
			this->SuspendLayout();
			// 
			// Ticker
			// 
			this->Ticker->Enabled = true;
			this->Ticker->Interval = 2;
			this->Ticker->Tick += gcnew System::EventHandler(this, &GUI::Update);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(12, 479);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(428, 26);
			this->button1->TabIndex = 3;
			this->button1->Text = L"Generate";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &GUI::button1_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(3, 43);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(71, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Floor strength";
			// 
			// FloorBar
			// 
			this->FloorBar->Location = System::Drawing::Point(6, 59);
			this->FloorBar->Maximum = 1000;
			this->FloorBar->Name = L"FloorBar";
			this->FloorBar->Size = System::Drawing::Size(204, 45);
			this->FloorBar->TabIndex = 9;
			this->FloorBar->TickFrequency = 10;
			this->FloorBar->Value = 100;
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Floor", L"Sphere", L"Caves"});
			this->comboBox1->Location = System::Drawing::Point(6, 19);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(248, 21);
			this->comboBox1->TabIndex = 10;
			this->comboBox1->Text = L"Floor";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(257, 43);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(72, 13);
			this->label3->TabIndex = 11;
			this->label3->Text = L"Sphere radius";
			// 
			// SphereBar
			// 
			this->SphereBar->Location = System::Drawing::Point(216, 58);
			this->SphereBar->Maximum = 1000;
			this->SphereBar->Name = L"SphereBar";
			this->SphereBar->Size = System::Drawing::Size(206, 45);
			this->SphereBar->TabIndex = 9;
			this->SphereBar->TickFrequency = 10;
			this->SphereBar->Value = 600;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->comboBox1);
			this->groupBox1->Controls->Add(this->SphereBar);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->FloorBar);
			this->groupBox1->Location = System::Drawing::Point(12, 13);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(428, 109);
			this->groupBox1->TabIndex = 12;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Generation type";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->label7);
			this->groupBox2->Controls->Add(this->label6);
			this->groupBox2->Controls->Add(this->label10);
			this->groupBox2->Controls->Add(this->label5);
			this->groupBox2->Controls->Add(this->label9);
			this->groupBox2->Controls->Add(this->label4);
			this->groupBox2->Controls->Add(this->label8);
			this->groupBox2->Controls->Add(this->label1);
			this->groupBox2->Controls->Add(this->WeightBar3);
			this->groupBox2->Controls->Add(this->WeightBar2);
			this->groupBox2->Controls->Add(this->NoiseBar3);
			this->groupBox2->Controls->Add(this->WeightBar1);
			this->groupBox2->Controls->Add(this->NoiseBar2);
			this->groupBox2->Controls->Add(this->NoiseBar1);
			this->groupBox2->Location = System::Drawing::Point(11, 128);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(428, 220);
			this->groupBox2->TabIndex = 13;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Sampling";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(202, 166);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(43, 13);
			this->label7->TabIndex = 5;
			this->label7->Text = L"^noisy^";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(4, 166);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(53, 13);
			this->label6->TabIndex = 5;
			this->label6->Text = L"^smooth^";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(258, 118);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(41, 13);
			this->label10->TabIndex = 5;
			this->label10->Text = L"Weight";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(4, 118);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(84, 13);
			this->label5->TabIndex = 5;
			this->label5->Text = L"Octave 3: Noise";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(258, 67);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(41, 13);
			this->label9->TabIndex = 5;
			this->label9->Text = L"Weight";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(4, 67);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(84, 13);
			this->label4->TabIndex = 5;
			this->label4->Text = L"Octave 2: Noise";
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(258, 16);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(41, 13);
			this->label8->TabIndex = 5;
			this->label8->Text = L"Weight";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(4, 16);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(84, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Octave 1: Noise";
			// 
			// WeightBar3
			// 
			this->WeightBar3->Location = System::Drawing::Point(254, 134);
			this->WeightBar3->Maximum = 1000;
			this->WeightBar3->Name = L"WeightBar3";
			this->WeightBar3->Size = System::Drawing::Size(168, 45);
			this->WeightBar3->TabIndex = 9;
			this->WeightBar3->TickFrequency = 10;
			this->WeightBar3->Value = 200;
			// 
			// WeightBar2
			// 
			this->WeightBar2->Location = System::Drawing::Point(254, 83);
			this->WeightBar2->Maximum = 1000;
			this->WeightBar2->Name = L"WeightBar2";
			this->WeightBar2->Size = System::Drawing::Size(168, 45);
			this->WeightBar2->TabIndex = 9;
			this->WeightBar2->TickFrequency = 10;
			this->WeightBar2->Value = 300;
			// 
			// NoiseBar3
			// 
			this->NoiseBar3->Location = System::Drawing::Point(7, 134);
			this->NoiseBar3->Maximum = 1000;
			this->NoiseBar3->Name = L"NoiseBar3";
			this->NoiseBar3->Size = System::Drawing::Size(248, 45);
			this->NoiseBar3->TabIndex = 9;
			this->NoiseBar3->TickFrequency = 10;
			this->NoiseBar3->Value = 600;
			// 
			// WeightBar1
			// 
			this->WeightBar1->Location = System::Drawing::Point(254, 32);
			this->WeightBar1->Maximum = 1000;
			this->WeightBar1->Name = L"WeightBar1";
			this->WeightBar1->Size = System::Drawing::Size(168, 45);
			this->WeightBar1->TabIndex = 9;
			this->WeightBar1->TickFrequency = 10;
			this->WeightBar1->Value = 400;
			// 
			// NoiseBar2
			// 
			this->NoiseBar2->Location = System::Drawing::Point(7, 83);
			this->NoiseBar2->Maximum = 1000;
			this->NoiseBar2->Name = L"NoiseBar2";
			this->NoiseBar2->Size = System::Drawing::Size(248, 45);
			this->NoiseBar2->TabIndex = 9;
			this->NoiseBar2->TickFrequency = 10;
			this->NoiseBar2->Value = 500;
			// 
			// NoiseBar1
			// 
			this->NoiseBar1->Location = System::Drawing::Point(7, 32);
			this->NoiseBar1->Maximum = 1000;
			this->NoiseBar1->Name = L"NoiseBar1";
			this->NoiseBar1->Size = System::Drawing::Size(248, 45);
			this->NoiseBar1->TabIndex = 9;
			this->NoiseBar1->TickFrequency = 10;
			this->NoiseBar1->Value = 400;
			// 
			// IsoValue
			// 
			this->IsoValue->Location = System::Drawing::Point(18, 428);
			this->IsoValue->Maximum = 1000;
			this->IsoValue->Minimum = 1;
			this->IsoValue->Name = L"IsoValue";
			this->IsoValue->Size = System::Drawing::Size(415, 45);
			this->IsoValue->TabIndex = 9;
			this->IsoValue->Value = 500;
			this->IsoValue->Scroll += gcnew System::EventHandler(this, &GUI::TerrainSize_Scroll_1);
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(15, 412);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(50, 13);
			this->label11->TabIndex = 5;
			this->label11->Text = L"Iso value";
			// 
			// UpdateCheckbox
			// 
			this->UpdateCheckbox->AutoSize = true;
			this->UpdateCheckbox->Location = System::Drawing::Point(30, 355);
			this->UpdateCheckbox->Name = L"UpdateCheckbox";
			this->UpdateCheckbox->Size = System::Drawing::Size(84, 17);
			this->UpdateCheckbox->TabIndex = 14;
			this->UpdateCheckbox->Text = L"Auto-update";
			this->UpdateCheckbox->UseVisualStyleBackColor = true;
			// 
			// GUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(451, 517);
			this->Controls->Add(this->UpdateCheckbox);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->IsoValue);
			this->Name = L"GUI";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GUI";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->FloorBar))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->SphereBar))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->WeightBar3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->WeightBar2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NoiseBar3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->WeightBar1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NoiseBar2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->NoiseBar1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->IsoValue))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Update(System::Object^  sender, System::EventArgs^  e);

	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e)
			 {
				 GenerateInfo genInfo;

				 if (comboBox1->Text == "Floor")
				 {
					 genInfo.genType = GM_FLOOR;
				 }
				 else if (comboBox1->Text == "Sphere")
				 {
					 genInfo.genType = GM_SPHERE;
				 }
				 else
				 {
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

				generateTerrain(genInfo);
			 }


private: System::Void TerrainSize_Scroll(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void TerrainSize_Scroll_1(System::Object^  sender, System::EventArgs^  e) {
		 }
};
}
