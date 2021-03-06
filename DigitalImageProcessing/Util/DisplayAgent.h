#pragma once
#include "../stdafx.h"

struct ThreadOption
{
	enum Type
	{
		Afx, OpenMP, OpenCL
	};
	Type type;
	int count;
};

class DisplayAgent
{
private:
	ULONGLONG _StartTime;

	static DisplayAgent displayAgent;
	DisplayAgent() = default;
public:
	~DisplayAgent() = default;
	static DisplayAgent* GetInstance();

	HWND		HWnd;
	CEdit		*OutputArea;
	CStatic		*PictureLeft;
	CStatic		*PictureRight;
	CComboBox	*ThreadType;
	CSliderCtrl *ThreadSlider;
	CImage		**DlgImage;

	void OutputLine(const CString &str);
	void ClearOutput(void);
	ThreadOption GetThreadOption(void);
	void PaintCImageToCStatic(CImage *i, CStatic *s);
	void StartTick(void);
	void PrintTimeElapsed(void);
};
#define DA (DisplayAgent::GetInstance())