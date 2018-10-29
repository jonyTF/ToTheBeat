#pragma once

enum
{
	ID_Hello = 1,
	ID_OPEN,
	ID_MEDIACTRL
};

class ToTheBeatApp : public wxApp
{
public:
	virtual bool OnInit() wxOVERRIDE;
};

class MainFrame : public wxFrame
{
public:
	MainFrame();

protected:
	wxMediaCtrl *m_mediactrl;
	wxPanel *m_panel_top;
	wxPanel *m_panel_bot;

private:
	void OnHello(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);

	void OnLoadVideo(wxMediaEvent& event);
	bool videoLoaded;

	void OnMouseEnter(wxMouseEvent& event);
	void OnMouseLeave(wxMouseEvent& event);
	void OnMouseMotion(wxMouseEvent& event);
	bool mouseInWindow;
};