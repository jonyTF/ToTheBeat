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
	wxMediaCtrl* m_mediactrl;

private:
	void OnHello(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
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