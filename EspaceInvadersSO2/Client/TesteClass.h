#pragma once


// TesteClass dialog

class TesteClass : public CDialogEx
{
	DECLARE_DYNAMIC(TesteClass)

public:
	TesteClass(CWnd* pParent = NULL);   // standard constructor
	virtual ~TesteClass();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROPPAGE_MEDIUM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
