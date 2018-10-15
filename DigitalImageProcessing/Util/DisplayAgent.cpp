#include "../stdafx.h"
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

void DisplayAgent::PaintCImageToCStatic(CImage *i, CStatic *s)
{
	int height, width;
	CRect rect, newrect;
	height = i->GetHeight();
	width = i->GetWidth();

	s->GetClientRect(&rect);
	CDC *pDC = s->GetDC();
	SetStretchBltMode(pDC->m_hDC, STRETCH_HALFTONE);

	if (width <= rect.Width() && height <= rect.Width())
	{
		newrect = CRect(rect.TopLeft(), CSize(width, height));
		i->StretchBlt(pDC->m_hDC, newrect, SRCCOPY);
	}
	else
	{
		float xScale = rect.Width() / (float)width;
		float yScale = rect.Height() / (float)height;
		float ScaleIndex = xScale <= yScale ? xScale : yScale;
		newrect = CRect(rect.TopLeft(), CSize(static_cast<int>(width * ScaleIndex), static_cast<int>(height * ScaleIndex)));
		i->StretchBlt(pDC->m_hDC, newrect, SRCCOPY);
	}
	s->ReleaseDC(pDC);
}

void DisplayAgent::StartTick(void)
{
	this->_StartTime = GetTickCount64();
}

void DisplayAgent::PrintTimeElapsed(void)
{
	CString str;
	str.Format(_T("Time elapsed: %d ms"), GetTickCount64() - this->_StartTime);
	this->OutputLine(str);
}
