#pragma once
#include "afxdialogex.h"
#include "pch.h"
#include <vector>
#include <string>

// ConditionItemDlg 대화 상자

class ConditionItemDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ConditionItemDlg)

public:

	ConditionItemDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.

	virtual ~ConditionItemDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONDITIONITEM };
#endif

	inline char GetConditionIndex() const { return conditionIndex; }

	void SetConditionIndex(const char index);

	afx_msg void OnBnClickedBtnDelete();

	afx_msg void OnStnClickedStaticCompleteText();

	void SetConditionText(const CString& completeText);

	std::string GetCompleteText();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:

	CMFCButton m_Btn_Delete;

	CStatic m_StaticCompleteText;

	char conditionIndex;

	CButton m_Check_Index;
};
