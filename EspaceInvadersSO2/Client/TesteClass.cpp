// TesteClass.cpp : implementation file
//

#include "stdafx.h"
#include "TesteClass.h"
#include "afxdialogex.h"


// TesteClass dialog

IMPLEMENT_DYNAMIC(TesteClass, CDialogEx)

TesteClass::TesteClass(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PROPPAGE_MEDIUM, pParent)
{

}

TesteClass::~TesteClass()
{
}

void TesteClass::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TesteClass, CDialogEx)
END_MESSAGE_MAP()


// TesteClass message handlers
