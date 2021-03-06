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
	m_videoLoaded = false;

	// -----Menu Bar-----
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
	
	// -----Layout-----
	wxSplitterWindow *splitter = new wxSplitterWindow(this);
	m_panelTop = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN);
	m_panelBot = new wxScrolled<wxPanel>(splitter, wxID_ANY);

	splitter->SplitHorizontally(m_panelTop, m_panelBot);
	splitter->SetMinimumPaneSize(300);

	// mPanelTop
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

	m_mediaCtrl = new wxMediaCtrl(m_panelTop, ID_MEDIACTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0L, wxMEDIABACKEND_WMP10);
	m_mediaCtrl->ShowPlayerControls();

	vbox->Add(m_mediaCtrl, 0, wxEXPAND | wxALL, 20);
	m_panelTop->SetSizer(vbox);

	// mPanelBot
	wxBoxSizer *vbox2 = new wxBoxSizer(wxVERTICAL);

	m_timeline = new Timeline(m_panelBot, this);

	vbox2->Add(m_timeline, 1, wxALL, 0);
	m_panelBot->SetSizer(vbox2);
	m_panelBot->FitInside();
	m_panelBot->SetScrollRate(5, 5);

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
		if (!m_mediaCtrl->Load(fd.GetPath()))
		{
			wxLogMessage("Failed to load " + fd.GetPath());
		}
	}
}

void MainFrame::onLoadVideo(wxMediaEvent& event)
{
	//wxLogMessage("LOADED VIDEO!");
	//m_mediactrl->Play();
	m_videoLoaded = true;
	m_timeline->updateSize();
}

Timeline::Timeline(wxWindow* parent, MainFrame* frame) : wxWindow(parent, wxID_ANY)
{
	m_parentFrame = frame;
	m_mouseInWindow = false;
	m_curVideoPos = 0;
	m_mouseX = 0;
	m_lastMouseX = 0;
	m_width = 1000;
	m_zoom = 1.f;

	SetMinSize(wxSize(m_width, -1));
	SetBackgroundColour(*wxRED);

	//GetSize(&mWidth, &mHeight);
	//SetSize(wxDefaultCoord, wxDefaultCoord, mWidth+1000, wxDefaultCoord);

	m_timer.Bind(wxEVT_TIMER, &Timeline::onTimer, this);

	Bind(wxEVT_ENTER_WINDOW, &Timeline::onMouseEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &Timeline::onMouseLeave, this);
	Bind(wxEVT_MOTION, &Timeline::onMouseMotion, this);
	Bind(wxEVT_LEFT_UP, &Timeline::onMouseLeftClick, this);
	Bind(wxEVT_MOUSEWHEEL, &Timeline::onMouseWheel, this);
	
	Bind(wxEVT_PAINT, &Timeline::onPaint, this);

	Bind(wxEVT_SIZE, &Timeline::onResize, this);
}

void Timeline::onMouseEnter(wxMouseEvent& event)
{
	m_mouseInWindow = true;
	wxLogDebug("Enterr!!");
	m_timer.Start(100);

	Refresh();
}

void Timeline::onMouseLeave(wxMouseEvent& event)
{
	m_mouseInWindow = false;
	wxLogDebug("EXIT!!!");
	m_timer.Stop();

	Refresh();
}

void Timeline::onMouseMotion(wxMouseEvent& event)
{
	int x = event.GetX();
	m_mouseX = x;

	float percent = (float)x / m_width;
	m_curVideoPos = percent * m_videoLength;

	wxLogDebug("Percent: %f", percent);

	Refresh();
}

void Timeline::onTimer(wxTimerEvent& event)
{
	wxPoint p = wxGetMousePosition();
	int x = p.x - GetScreenPosition().x;

	if (m_lastMouseX != x)
	{
		m_lastMouseX = x;

		// Display the current frame of the video with a timer
		if (m_parentFrame->m_videoLoaded)
		{
			m_parentFrame->m_mediaCtrl->Seek(m_curVideoPos);
			m_parentFrame->m_mediaCtrl->Play();
			m_parentFrame->m_mediaCtrl->Pause();
		}
	}
}

void Timeline::onMouseLeftClick(wxMouseEvent& event)
{
	if (m_parentFrame->m_videoLoaded)
	{
		m_positions.push_back(m_curVideoPos);
		Refresh();
	}
}

void Timeline::onMouseWheel(wxMouseEvent& event)
{
	//wxLogDebug("wheel rotation: %d", event.GetWheelRotation());
	int rot = event.GetWheelRotation();
	if (rot > 0)
	{
		m_zoom += 0.1f;
	}
	else
	{
		if (m_zoom - 0.1f > 0)
			m_zoom -= 0.1f;
	}

	updateSize(false);
}

void Timeline::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);
	//dc.SetBrush(*wxGREEN_BRUSH);

	if (m_mouseInWindow)
	{
		// Draw the current time cursor
		dc.SetPen(wxPen(wxColor(255, 255, 0), 2));
		dc.DrawLine(m_mouseX, 0, m_mouseX, m_height);
	}

	// Draw the markers that have been placed
	for (std::vector<int>::iterator it = m_positions.begin(); it != m_positions.end(); ++it)
	{
		drawMarker(dc, *it);
	}

	// Draw the time stamps
	dc.SetPen(wxPen(wxColor(200, 200, 200), 2));
	dc.SetBrush(wxBrush(wxColor(200, 200, 200)));
	dc.DrawRectangle(0, 0, m_width, 20);

	dc.SetPen(wxPen(*wxBLACK, 1));
	for (int ms = 0; ms < m_videoLength; ms += 100)
	{
		int x;
		if (ms % 1000 == 0)
		{
			x = ms / 1000 * m_zoom * 100;
			dc.DrawLine(x, 0, x, 20);

			// Get timestamp hh:mm:ss
			char timestamp[9];
			getTimestamp(timestamp, ms);
			dc.DrawText(timestamp, x, 0);
		}
		else
		{
			x = ms / 100 * m_zoom * 10;
			dc.DrawLine(x, 10, x, 20);
		}
	}
}

void Timeline::getTimestamp(char *buf, int ms)
{
	int s, m, h;
	s = ms / 1000;
	m = s / 60;
	h = m / 60;
	s %= 60;
	m %= 60;

	std::string fmt;
	if (h / 10 == 0)
		fmt.push_back('0');
	fmt.append("%d:");
	if (m / 10 == 0)
		fmt.push_back('0');
	fmt.append("%d:");
	if (s / 10 == 0)
		fmt.push_back('0');
	fmt.append("%d");

	sprintf(buf, fmt.c_str(), h, m, s);
}

void Timeline::drawMarker(wxDC& dc, int pos)
{
	int x = (float)pos/m_videoLength*m_width;

	dc.SetPen(wxPen(wxColor(255, 255, 255), 2));
	dc.DrawLine(x, 0, x, m_height);
}

void Timeline::onResize(wxSizeEvent& event)
{
	GetSize(nullptr, &m_height);
	wxLogDebug("mWidth: %d, mHeight: %d", m_width, m_height);
}

void Timeline::updateSize(bool newVideo)
{
	if (newVideo)
	{
		m_positions.clear();
		m_videoLength = m_parentFrame->m_mediaCtrl->Length();
	}
	m_width = m_zoom * (m_videoLength / 10);
	SetMinSize(wxSize(m_width, -1));
	SetSize(wxSize(m_width, -1));

	// Update scrollbars
	GetParent()->FitInside();

	Refresh();
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