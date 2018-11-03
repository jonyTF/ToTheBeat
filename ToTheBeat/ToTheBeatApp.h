#pragma once

enum
{
	ID_Hello = 1,
	ID_OPEN,
	ID_MEDIACTRL
};

class MainPanel;
class Timeline;
class MainFrame;

class ToTheBeatApp : public wxApp
{
public:
	virtual bool OnInit() wxOVERRIDE;
};

class MainPanel : public wxPanel
{
public:
	MainPanel(wxWindow* parent);
};

class Timeline : public MainPanel
{
public:
	Timeline(wxWindow* parent, MainFrame* frame);

	bool mMouseInWindow;
	int mCurVideoPos;
	std::vector<int> mPositions;

private:
	void onPaint(wxPaintEvent& event);
	void onMouseEnter(wxMouseEvent& event);
	void onMouseLeave(wxMouseEvent& event);
	void onMouseMotion(wxMouseEvent& event);
	void onMouseLeftClick(wxMouseEvent& event);

	MainFrame *mParentFrame;
};

class MainFrame : public wxFrame
{
public:
	MainFrame();
	wxMediaCtrl *mMediaCtrl;
	MainPanel *mPanelTop;
	Timeline *mPanelBot;

	bool mVideoLoaded;

private:
	void onHello(wxCommandEvent& event);
	void onOpen(wxCommandEvent& event);
	void onExit(wxCommandEvent& event);
	void onAbout(wxCommandEvent& event);

	void onLoadVideo(wxMediaEvent& event);
};