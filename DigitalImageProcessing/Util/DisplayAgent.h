#pragma once
#include "afxwin.h"
#include "afxcmn.h"

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
};