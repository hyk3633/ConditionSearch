﻿
// ConditionSearchDlg.h: 헤더 파일

#pragma once

#include "afxbutton.h"
#include <memory>
#include <string>
#include <fstream>
#include <sstream>
#include "XMLParser.h"
#include "CSVParser.h"
#include "JsonParser.h"

#include "../ColorChangeDLL/ColorChangeDlg.h"
#pragma comment (lib, "./x64/Debug/ColorChangeDLL.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

class ConditionItemDlg;

// CConditionSearchDlg 대화 상자
class CConditionSearchDlg : public CDialogEx
{
// 생성입니다.
public:
	CConditionSearchDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONDITIONSEARCH_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:

	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();

	void LoadControlStatus();

	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT MsgDeleteConditionItem(WPARAM wParam, LPARAM lParam);

	void ClearCurrentControls();

	CString GetItemText();

public:

	afx_msg void OnTvnSelchangedTreeConditionlist(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnNMDblclkTreeConditionlist(NMHDR* pNMHDR, LRESULT* pResult);

protected:

	bool CheckIsCondition();

	void CreateControls(const std::vector<ControlAttribute>& controlInfo);

	void SetControlFontSize(CWnd* pParentWnd, CWnd* pControl, int fontSize);

public:

	afx_msg void OnBtnClickedBtnAdd();

protected:

	void MakeCompleteText();

	void ReplacePakcetIdToValue(const EControlType controlType, const int ctrlId, const std::string& packetId, std::string& completeText);

	void CreateConditionDialog();

	void SaveCtrlValue();

public:

	afx_msg void OnBtnClickedBtnModify();

	afx_msg void OnBtnClickedBtnSearch();

protected:

	void ClearListControl();

public:

	afx_msg void OnCondSave();
	afx_msg void OnCondLoad();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSetColor();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

protected:

	void SetToPreviewMode();

public:

	inline COLORREF* GetFontColor() { return &currentFontColor; }
	inline COLORREF* GetBgColor() { return &currentBgColor; }
	inline std::vector<CWnd*>& GetFontColorControls() { return fontColorControls; }

private:

	CTreeCtrl m_TreeCtrl;

	CMFCButton m_BtnModify;

	CMFCButton m_BtnAdd;

	CMFCButton m_Btn_Search;

	CListCtrl m_ListCtrl;

	ConditionItemDlg* m_ConditionItem;

	std::unique_ptr<XMLParser> xmlParserPtr;

	std::unique_ptr<CSVParser> csvParserPtr;

	std::unique_ptr<JsonParser> jsonParserPtr;

	std::vector<std::pair<CWnd*, ControlAttribute>> currentControls;

	std::vector<std::pair<int, std::string>> savedControlValues;

	std::vector<HTREEITEM> highlightedItems;

	std::vector<std::string> currentCtrlValues;

	std::string addedConditionId;

	std::string completeText;

	CString completeTextCStr;

	CString conditionSavePath = L"..//Condition";

	CRect m_rcMinimumDialog;

	int leftStart;
	int topStart;

	ColorChangeDlg* colorChangeDlg;

	std::vector<CWnd*> fontColorControls;

	COLORREF currentFontColor;
	COLORREF currentBgColor;

	CConditionSearchDlg* previewDlg;

	bool bPreviewMode = false;

};
