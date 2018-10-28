#include <wx/wxprec.h>
#include <wx/splitter.h>

#include <wx/mediactrl.h>   // for wxMediaCtrl
#include <wx/filedlg.h>		// for opening files from OpenFile
#include <wx/slider.h>		// for a slider for seeking within media
#include <wx/sizer.h>		// for positioning controls/wxBoxSizer
#include <wx/timer.h>		// timer for updating status bar
#include <wx/textdlg.h>		// for getting user text from OpenURL/Debug
#include <wx/notebook.h>	// for wxNotebook and putting movies in pages
#include <wx/cmdline.h>		// for wxCmdLineParser (optional)
#include <wx/listctrl.h>	// for wxListCtrl
#include <wx/dnd.h>			// drag and drop for the playlist
#include <wx/filename.h>	// For wxFileName::GetName()
#include <wx/config.h>		// for native wxConfig
#include <wx/vector.h>

#include "ToTheBeatApp.h"

#ifdef _DEBUG
#pragma comment(lib, "wxbase31ud.lib")
#else
#pragma comment(lib, "wxbase31u.lib")
#endif

/*const wxString wxGetMediaStateText(int nState)
{
	switch (nState)
	{
		case wxMEDIASTATE_PLAYING:
			return "Playing";
		case wxMEDIASTATE_STOPPED:
			return "Stopped";
		///case wxMEDIASTATE_PAUSED:
		default:
			return "Paused";
	}
}*/

bool ToTheBeatApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	SetAppName("ToTheBeat");

	MainFrame *mainFrame = new MainFrame();

	mainFrame->Show(true);
	return true;
}

TopPanel::TopPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_SUNKEN)
{
	//SetBackgroundColour("gray");
	wxButton* b = new wxButton(this, wxID_ANY, "Le Top Button");
}

BottomPanel::BottomPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_SUNKEN)
{
	//SetBackgroundColour("gray");
	wxButton* b = new wxButton(this, wxID_ANY, "Le Bottom Button");
}

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "To The Beat")
{
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello....little friend\tCtrl-H");
	menuFile->Append(ID_OPEN, "&Open\tCtrl-O", "Open a video file or ttb project");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to the BEAT!");

	Bind(wxEVT_MENU, &MainFrame::OnHello, this, ID_Hello);
	Bind(wxEVT_MENU, &MainFrame::OnOpen, this, ID_OPEN);
	Bind(wxEVT_MENU, &MainFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MainFrame::OnExit,  this, wxID_EXIT);

	/*
	wxPanel *parent = new wxPanel(this);

	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

	TopPanel *top = new TopPanel(parent);
	BottomPanel *bottom = new BottomPanel(parent);

	vbox->Add(top, 1, wxEXPAND | wxALL, 5);
	vbox->Add(bottom, 1, wxEXPAND | wxALL, 5);

	parent->SetSizer(vbox);

	this->Center();
	*/
	
	wxSplitterWindow *splitter = new wxSplitterWindow(this);

	TopPanel *top = new TopPanel(splitter);
	BottomPanel *bottom = new BottomPanel(splitter);

	splitter->SplitHorizontally(top, bottom);
	splitter->SetMinimumPaneSize(100);

	m_mediactrl = new wxMediaCtrl();

	bool bOK = m_mediactrl->Create(this, ID_MEDIACTRL);
	wxASSERT_MSG(bOK, "Could not create wxMediaCtrl!");
	wxUnusedVar(bOK);


}

void MainFrame::OnExit(wxCommandEvent& event)
{
	Close(true);
}

void MainFrame::OnAbout(wxCommandEvent& event)
{
	wxMessageBox("This is an app that syncs videos with the selected music.",
				"About ToTheBeat", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnHello(wxCommandEvent& event)
{
	wxLogMessage("Hello CRUEL world...");
}

void MainFrame::OnOpen(wxCommandEvent& event)
{
	wxFileDialog fd(this);
	if (fd.ShowModal() == wxID_OK)
	{
		wxLogMessage("lol" + fd.GetPath());
	}
}

wxIMPLEMENT_APP(ToTheBeatApp);