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

	inline std::string GetConditionIndex() const { return index; }

	void SetConditionIndex(const std::string& idx);

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

	std::string index;

	CButton m_Check_Index;
	afx_msg void OnBnClickedBtnUp();
	afx_msg void OnBnClickedBtnDown();
};
