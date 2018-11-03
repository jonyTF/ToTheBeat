#include <wx/wxprec.h>
#include <wx/splitter.h>

#include <wx/mediactrl.h>   // for wxMediaCtrl
#include <wx/filedlg.h>		// for opening files from OpenFile
#include <wx/sizer.h>		// for positioning controls/wxBoxSizer
#include <wx/timer.h>		// timer for updating status bar
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
	mVideoLoaded = false;

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

	Bind(wxEVT_MENU, &MainFrame::onHello, this, ID_Hello);
	Bind(wxEVT_MENU, &MainFrame::onOpen, this, ID_OPEN);
	Bind(wxEVT_MENU, &MainFrame::onAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MainFrame::onExit,  this, wxID_EXIT);

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
	mPanelTop = new MainPanel(splitter);
	
	mPanelBot = new Timeline(splitter, this);

	splitter->SplitHorizontally(mPanelTop, mPanelBot);
	splitter->SetMinimumPaneSize(300);

	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	mPanelTop->SetSizer(vbox);

	mMediaCtrl = new wxMediaCtrl(mPanelTop, ID_MEDIACTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0L, wxMEDIABACKEND_WMP10);
	mMediaCtrl->ShowPlayerControls();

	vbox->Add(mMediaCtrl, 0, wxEXPAND | wxALL, 20);

	// Workaround for wxEVT_MEDIA_LOADED not firing
	Bind(wxEVT_MEDIA_STOP, &MainFrame::onLoadVideo, this, ID_MEDIACTRL);

	Centre();
}

void MainFrame::onExit(wxCommandEvent& event)
{
	Close(true);
}

void MainFrame::onAbout(wxCommandEvent& event)
{
	wxMessageBox("This is an app that syncs videos with the selected music.",
				"About ToTheBeat", wxOK | wxICON_INFORMATION);
}

void MainFrame::onHello(wxCommandEvent& event)
{
	wxLogMessage("Hello CRUEL world...");
	wxLogDebug("watt");
}

void MainFrame::onOpen(wxCommandEvent& event)
{
	wxFileDialog fd(this);
	if (fd.ShowModal() == wxID_OK)
	{
		if (!mMediaCtrl->Load(fd.GetPath()))
		{
			wxLogMessage("Failed to load " + fd.GetPath());
		}
	}
}

void MainFrame::onLoadVideo(wxMediaEvent& event)
{
	//wxLogMessage("LOADED VIDEO!");
	//m_mediactrl->Play();
	mVideoLoaded = true;
}

MainPanel::MainPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN)
{
}

Timeline::Timeline(wxWindow* parent, MainFrame* frame) : MainPanel(parent)
{
	mParentFrame = frame;
	mMouseInWindow = false;
	mCurVideoPos = 0;

	Bind(wxEVT_ENTER_WINDOW, &Timeline::onMouseEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &Timeline::onMouseLeave, this);
	Bind(wxEVT_MOTION, &Timeline::onMouseMotion, this);
	Bind(wxEVT_LEFT_DCLICK, &Timeline::onMouseLeftClick, this);
	Bind(wxEVT_PAINT, &Timeline::onPaint, this);
}

void Timeline::onMouseEnter(wxMouseEvent& event)
{
	mMouseInWindow = true;
	wxLogDebug("Enterr!!");
}

void Timeline::onMouseLeave(wxMouseEvent& event)
{
	mMouseInWindow = false;
	wxLogDebug("EXIT!!!");
}

void Timeline::onMouseMotion(wxMouseEvent& event)
{
	int x = event.GetX();
	//int y = event.GetY();

	int panel_width, panel_height;
	mParentFrame->mPanelBot->GetSize(&panel_width, &panel_height);

	float percent = (float)x / panel_width;
	int length = mParentFrame->mMediaCtrl->Length();
	mCurVideoPos = percent * length;

	wxLogDebug("Percent: %f", percent);

	if (mParentFrame->mVideoLoaded)
	{
		mParentFrame->mMediaCtrl->Seek(mCurVideoPos);
		// TODO: speed this up.
		// It is slow currently because it plays and pauses for EVERY SINGLE value that the mouse moved to
		mParentFrame->mMediaCtrl->Play();
		mParentFrame->mMediaCtrl->Pause();
	}
}

void Timeline::onMouseLeftClick(wxMouseEvent& event)
{
	mPositions.push_back(mCurVideoPos);
}

void Timeline::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	dc.SetBrush(*wxGREEN_BRUSH);
	dc.SetPen( wxPen( wxColor(255, 0, 255), 10 ) );
	dc.DrawCircle( wxPoint(50, 50), 25 );
}

wxIMPLEMENT_APP(ToTheBeatApp);

//ffmpeg stuff
/*

./ffmpeg
-i "INPUT_VIDEO" -filter_complex 
"[0:v]trim=start=0:end=5,setpts=(1/5)*PTS[a];
[0:v]trim=start=5:end=13,setpts=(1/8)*(PTS-STARTPTS)[b];
[0:v]trim=start=13:end=16,setpts=(1/3)*(PTS-STARTPTS)[c];
[0:v]trim=start=16:end=20,setpts=(1/4)*(PTS-STARTPTS)[d];
[a][b][c][d]concat=n=4[out1]" 
-map [out1] output.mp4

*/