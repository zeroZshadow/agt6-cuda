#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


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

	private: System::Windows::Forms::TrackBar^  trackBar1;
	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TrackBar^  trackBar2;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::GroupBox^  groupBox2;
	private: System::Windows::Forms::ComboBox^  comboBox2;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TrackBar^  trackBar5;
	private: System::Windows::Forms::TrackBar^  trackBar4;
	private: System::Windows::Forms::TrackBar^  trackBar3;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label6;


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
			this->trackBar1 = (gcnew System::Windows::Forms::TrackBar());
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->trackBar2 = (gcnew System::Windows::Forms::TrackBar());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->trackBar3 = (gcnew System::Windows::Forms::TrackBar());
			this->trackBar4 = (gcnew System::Windows::Forms::TrackBar());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->trackBar5 = (gcnew System::Windows::Forms::TrackBar());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar2))->BeginInit();
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar3))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar4))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar5))->BeginInit();
			this->SuspendLayout();
			// 
			// Ticker
			// 
			this->Ticker->Enabled = true;
			this->Ticker->Interval = 1;
			this->Ticker->Tick += gcnew System::EventHandler(this, &GUI::Update);
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(12, 447);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(260, 26);
			this->button1->TabIndex = 3;
			this->button1->Text = L"Generate";
			this->button1->UseVisualStyleBackColor = true;
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
			// trackBar1
			// 
			this->trackBar1->Location = System::Drawing::Point(6, 59);
			this->trackBar1->Name = L"trackBar1";
			this->trackBar1->Size = System::Drawing::Size(248, 45);
			this->trackBar1->TabIndex = 9;
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"Floor", L"Sphere", L"Caves"});
			this->comboBox1->Location = System::Drawing::Point(6, 19);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(248, 21);
			this->comboBox1->TabIndex = 10;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(3, 91);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(62, 13);
			this->label3->TabIndex = 11;
			this->label3->Text = L"Sphere size";
			// 
			// trackBar2
			// 
			this->trackBar2->Location = System::Drawing::Point(6, 107);
			this->trackBar2->Name = L"trackBar2";
			this->trackBar2->Size = System::Drawing::Size(248, 45);
			this->trackBar2->TabIndex = 9;
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->comboBox1);
			this->groupBox1->Controls->Add(this->trackBar2);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->trackBar1);
			this->groupBox1->Location = System::Drawing::Point(12, 13);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(260, 158);
			this->groupBox1->TabIndex = 12;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Generation type";
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->comboBox2);
			this->groupBox2->Controls->Add(this->label7);
			this->groupBox2->Controls->Add(this->label6);
			this->groupBox2->Controls->Add(this->label5);
			this->groupBox2->Controls->Add(this->label4);
			this->groupBox2->Controls->Add(this->label1);
			this->groupBox2->Controls->Add(this->trackBar5);
			this->groupBox2->Controls->Add(this->trackBar4);
			this->groupBox2->Controls->Add(this->trackBar3);
			this->groupBox2->Location = System::Drawing::Point(12, 178);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(260, 263);
			this->groupBox2->TabIndex = 13;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Sampling";
			// 
			// comboBox2
			// 
			this->comboBox2->FormattingEnabled = true;
			this->comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"Premade Perlin", L"Perlin (slow)"});
			this->comboBox2->Location = System::Drawing::Point(6, 20);
			this->comboBox2->Name = L"comboBox2";
			this->comboBox2->Size = System::Drawing::Size(248, 21);
			this->comboBox2->TabIndex = 0;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(3, 44);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(51, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Octave 1";
			// 
			// trackBar3
			// 
			this->trackBar3->Location = System::Drawing::Point(6, 60);
			this->trackBar3->Name = L"trackBar3";
			this->trackBar3->Size = System::Drawing::Size(248, 45);
			this->trackBar3->TabIndex = 9;
			// 
			// trackBar4
			// 
			this->trackBar4->Location = System::Drawing::Point(6, 111);
			this->trackBar4->Name = L"trackBar4";
			this->trackBar4->Size = System::Drawing::Size(248, 45);
			this->trackBar4->TabIndex = 9;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(3, 95);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(51, 13);
			this->label4->TabIndex = 5;
			this->label4->Text = L"Octave 2";
			// 
			// trackBar5
			// 
			this->trackBar5->Location = System::Drawing::Point(6, 162);
			this->trackBar5->Name = L"trackBar5";
			this->trackBar5->Size = System::Drawing::Size(248, 45);
			this->trackBar5->TabIndex = 9;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(3, 146);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(51, 13);
			this->label5->TabIndex = 5;
			this->label5->Text = L"Octave 3";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(3, 194);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(53, 13);
			this->label6->TabIndex = 5;
			this->label6->Text = L"^smooth^";
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(201, 194);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(43, 13);
			this->label7->TabIndex = 5;
			this->label7->Text = L"^noisy^";
			// 
			// GUI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 482);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->button1);
			this->Name = L"GUI";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = L"GUI";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar2))->EndInit();
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->groupBox2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar3))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar4))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->trackBar5))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void Update(System::Object^  sender, System::EventArgs^  e);

	};
}
