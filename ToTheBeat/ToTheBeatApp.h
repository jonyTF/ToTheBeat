#pragma once

enum
{
	ID_Hello = 1,
	ID_OPEN,
	ID_MEDIACTRL
};

class Timeline;
class MainFrame;

class ToTheBeatApp : public wxApp
{
public:
	virtual bool OnInit() wxOVERRIDE;
};

class Timeline : public wxWindow
{
public:
	Timeline(wxWindow* parent, MainFrame* frame);

	bool m_mouseInWindow;
	int m_curVideoPos;
	int m_mouseX;
	std::vector<int> m_positions;

	void updateSize();

private:
	MainFrame *m_parentFrame;
	wxTimer m_timer;

	int m_width;
	int m_height;
	int m_lastMouseX;

	void drawMarker(wxDC& dc, int pos);

	void onPaint(wxPaintEvent& event);
	void onMouseEnter(wxMouseEvent& event);
	void onMouseLeave(wxMouseEvent& event);
	void onMouseMotion(wxMouseEvent& event);
	void onMouseLeftClick(wxMouseEvent& event);
	void onResize(wxSizeEvent& event);
	void onTimer(wxTimerEvent& event);
};

class MainFrame : public wxFrame
{
public:
	MainFrame();
	wxMediaCtrl *m_mediaCtrl;
	wxPanel *m_panelTop;
	wxScrolled<wxPanel> *m_panelBot;
	Timeline *m_timeline;

	bool m_videoLoaded;

private:
	void onHello(wxCommandEvent& event);
	void onOpen(wxCommandEvent& event);
	void onExit(wxCommandEvent& event);
	void onAbout(wxCommandEvent& event);

	void onLoadVideo(wxMediaEvent& event);
};