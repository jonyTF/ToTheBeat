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

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "To The Beat", wxDefaultPosition, wxSize(800, 600))
{
	videoLoaded = false;

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
	
	// -----Layout-----
	wxSplitterWindow *splitter = new wxSplitterWindow(this);
	m_panel_top = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
	
	m_panel_bot = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);

	splitter->SplitHorizontally(m_panel_top, m_panel_bot);
	splitter->SetMinimumPaneSize(300);

	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	m_panel_top->SetSizer(vbox);

	m_mediactrl = new wxMediaCtrl(m_panel_top, ID_MEDIACTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0L, wxMEDIABACKEND_WMP10);
	m_mediactrl->ShowPlayerControls();

	vbox->Add(m_mediactrl, 0, wxEXPAND | wxALL, 20);

	// Workaround for wxEVT_MEDIA_LOADED not firing
	Bind(wxEVT_MEDIA_STOP, &MainFrame::OnLoadVideo, this, ID_MEDIACTRL);


	// -----Mouse Stuff-----
	m_panel_bot->Bind(wxEVT_ENTER_WINDOW, &MainFrame::OnMouseEnter, this);
	m_panel_bot->Bind(wxEVT_LEAVE_WINDOW, &MainFrame::OnMouseLeave, this);
	m_panel_bot->Bind(wxEVT_MOTION, &MainFrame::OnMouseMotion, this);

	Centre();
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
	wxLogDebug("watt");
}

void MainFrame::OnOpen(wxCommandEvent& event)
{
	wxFileDialog fd(this);
	if (fd.ShowModal() == wxID_OK)
	{
		if (!m_mediactrl->Load(fd.GetPath()))
		{
			wxLogMessage("Failed to load " + fd.GetPath());
		}
	}
}

void MainFrame::OnLoadVideo(wxMediaEvent& event)
{
	//wxLogMessage("LOADED VIDEO!");
	//m_mediactrl->Play();
	videoLoaded = true;
}

void MainFrame::OnMouseEnter(wxMouseEvent& event)
{
	mouseInWindow = true;
	wxLogDebug("Enterr!!");
}

void MainFrame::OnMouseLeave(wxMouseEvent& event)
{
	mouseInWindow = false;
	wxLogDebug("EXIT!!!");
}

void MainFrame::OnMouseMotion(wxMouseEvent& event)
{
	int x = event.GetX();
	//int y = event.GetY();

	int panel_width, panel_height;
	m_panel_bot->GetSize(&panel_width, &panel_height);

	float percent = (float)x / panel_width;
	int length = m_mediactrl->Length();

	wxLogDebug("Percent: %f", percent);

	if (videoLoaded)
	{
		m_mediactrl->Seek(percent * length);
		// TODO: speed this up.
		// It is slow currently because it plays and pauses for EVERY SINGLE value that the mouse moved to
		m_mediactrl->Play();
		m_mediactrl->Pause();
	}
}

wxIMPLEMENT_APP(ToTheBeatApp);