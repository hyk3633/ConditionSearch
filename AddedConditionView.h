#pragma once
#include <string>
#include <unordered_map>
#include <vector>

// AddedConditionView 폼 보기

#define ITEM_GAP 50
#define ITEM_ITER std::vector<std::pair<std::string, ConditionItemDlg*>>::iterator

class ConditionItemDlg;

class AddedConditionView : public CFormView
{
	DECLARE_DYNCREATE(AddedConditionView)

public:

	AddedConditionView();
	virtual ~AddedConditionView();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT MsgDeleteConditionItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgUpConditionItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgDownConditionItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgClickConditionItem(WPARAM wParam, LPARAM lParam);

	int GetTop(ConditionItemDlg* item);

	void DeleteConditionItem(const std::string& index);

	void RearrangeItems(ITEM_ITER deletedIt, const int deletedTop);

	void DecreaseCount();

public:

	virtual void OnInitialUpdate();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	void Add(const std::string& index, const CString& completeText);

protected:

	int GetTopPosition();

private:

	int count = 0;
	CSize sizeTotal;
	std::unordered_map<std::string, ConditionItemDlg*> addedConditionDlgMap;
	std::vector<std::pair<std::string, ConditionItemDlg*>> addedConditionDlgVec;
};


