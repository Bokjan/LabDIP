
// DigitalImageProcessing.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDigitalImageProcessingApp: 
// �йش����ʵ�֣������ DigitalImageProcessing.cpp
//

class CDigitalImageProcessingApp : public CWinApp
{
public:
	CDigitalImageProcessingApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDigitalImageProcessingApp theApp;