#pragma once

enum
{
	ID_Hello = 1
};

class ToTheBeatApp : public wxApp
{
public:
#ifdef __WXMAC__
	virtual void MacOpenFiles(const wxArrayString & fileNames) wxOVERRIDE;
#endif

	virtual bool OnInit() wxOVERRIDE;

protected:
	class wxMediaPlayerFrame *m_frame;
};

class MainFrame : public wxFrame
{
public:
	MainFrame();

private:
	void OnHello(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
};

class TopPanel : public wxPanel
{
public:
	TopPanel(wxWindow *parent);
};

class BottomPanel : public wxPanel
{
public:
	BottomPanel(wxWindow *parent);
};