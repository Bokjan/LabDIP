#include "DisplayAgent.h"

DisplayAgent DisplayAgent::displayAgent;

DisplayAgent * DisplayAgent::GetInstance()
{
	return &DisplayAgent::displayAgent;
}

void DisplayAgent::OutputLine(const CString &str)
{
	this->OutputArea->SendMessage(WM_VSCROLL, SB_BOTTOM, 0);
	this->OutputArea->SetSel(this->OutputArea->GetWindowTextLength(), this->OutputArea->GetWindowTextLength(), FALSE);
	this->OutputArea->ReplaceSel(str + _T("\r\n"));
}

void DisplayAgent::ClearOutput(void)
{
	this->OutputArea->SetWindowTextW(_T(""));
}

ThreadOption DisplayAgent::GetThreadOption(void)
{
	ThreadOption ret;
	ret.count = this->ThreadSlider->GetPos();
	switch (this->ThreadType->GetCurSel())
	{
	case 0:
		ret.type = ThreadOption::Afx;
		break;
	case 1:
		ret.type = ThreadOption::OpenMP;
		break;
	}
	return ret;
}