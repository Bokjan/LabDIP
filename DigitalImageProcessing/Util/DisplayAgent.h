#pragma once
#include "afxwin.h"
#include "afxcmn.h"

struct ThreadOption
{
	enum Type
	{
		Afx, OpenMP
	};
	Type type;
	int count;
};

class DisplayAgent
{
private:
	static DisplayAgent displayAgent;
	DisplayAgent() = default;
public:
	~DisplayAgent() = default;
	static DisplayAgent* GetInstance();

	CEdit *OutputArea;
	CStatic *PictureLeft, *PictureRight;
	CComboBox *ThreadType;
	CSliderCtrl *ThreadSlider;

	void OutputLine(const CString &str);
	void ClearOutput(void);
	ThreadOption GetThreadOption(void);
};