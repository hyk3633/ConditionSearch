// AddedConditionView.cpp: 구현 파일
//

#include "pch.h"
#include "ConditionSearch.h"
#include "AddedConditionView.h"
#include "ConditionItemDlg.h"
#include <iostream>
#include <algorithm>

IMPLEMENT_DYNCREATE(AddedConditionView, CFormView)

#define FIND(vec, idx) std::find_if(vec.begin(), vec.end(), [&idx](std::pair<std::string, ConditionItemDlg*>& p) { return p.first == idx; })
#define REMOVE(vec, idx) vec.erase(std::remove_if(vec.begin(), vec.end(), [&idx](std::pair<std::string, ConditionItemDlg*>& p) { return p.first == idx; }))

AddedConditionView::AddedConditionView()
	: CFormView(IDD_FORMVIEW)
{

}

AddedConditionView::~AddedConditionView()
{
}

void AddedConditionView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

LRESULT AddedConditionView::MsgDeleteConditionItem(WPARAM wParam, LPARAM lParam)
{
	const std::string index((char*)wParam);
	DeleteConditionItem(index);
	GetParent()->SendMessage(CM_DELETE_CONDITIONITEM, (WPARAM)&index[0], 1);
	return LRESULT();
}

LRESULT AddedConditionView::MsgUpConditionItem(WPARAM wParam, LPARAM lParam)
{
	const std::string index((char*)wParam);

	auto cur = FIND(addedConditionDlgVec, index);
	if (cur == addedConditionDlgVec.begin())
		return LRESULT();

	auto prev = (cur - 1);
	const int prevTop = GetTop(prev->second);
	
	const int curTop = GetTop(cur->second);

	cur->second->SetWindowPos(NULL, 0, prevTop, 0, 0, SWP_NOSIZE);
	prev->second->SetWindowPos(NULL, 0, curTop, 0, 0, SWP_NOSIZE);

	std::iter_swap(cur, prev);

	return LRESULT();
}

LRESULT AddedConditionView::MsgDownConditionItem(WPARAM wParam, LPARAM lParam)
{
	const std::string index((char*)wParam);

	auto cur = FIND(addedConditionDlgVec, index);
	if ((cur+1) == addedConditionDlgVec.end())
		return LRESULT();

	auto next = (cur + 1);
	const int nextTop = GetTop(next->second);

	const int curTop = GetTop(cur->second);

	cur->second->SetWindowPos(NULL, 0, nextTop, 0, 0, SWP_NOSIZE);
	next->second->SetWindowPos(NULL, 0, curTop, 0, 0, SWP_NOSIZE);

	std::iter_swap(cur, next);

	return LRESULT();
}

int AddedConditionView::GetTop(ConditionItemDlg* item)
{
	if (item == nullptr)
		return 0;

	CRect rect;
	item->GetWindowRect(rect);
	ScreenToClient(rect);
	return rect.top;
}

void AddedConditionView::DeleteConditionItem(const std::string& index)
{
	if (addedConditionDlgMap.find(index) == addedConditionDlgMap.end())
		return;

	const auto conditionToDelete = addedConditionDlgMap[index];
	const int top = GetTop(conditionToDelete);

	conditionToDelete->DestroyWindow();

	// 나머지 컨디션 위치 조정
	auto deletedIt = FIND(addedConditionDlgVec, index);
	RearrangeItems(deletedIt, top);

	REMOVE(addedConditionDlgVec, index);
	addedConditionDlgMap.erase(index);

	delete conditionToDelete;

	// 개수 변수 조정
	DecreaseCount();
}

void AddedConditionView::RearrangeItems(ITEM_ITER deletedIt, const int deletedTop)
{
	int i = 0;
	for (auto it = deletedIt + 1; it != addedConditionDlgVec.end(); ++it)
	{
		const int newTopPos = deletedTop + i++ * ITEM_GAP;
		it->second->SetWindowPos(NULL, 0, newTopPos, 0, 0, SWP_NOSIZE);
	}
}

void AddedConditionView::DecreaseCount()
{
	count--;
}

BEGIN_MESSAGE_MAP(AddedConditionView, CFormView)
	ON_MESSAGE(CM_DELETE_CONDITIONITEM, &AddedConditionView::MsgDeleteConditionItem)
	ON_MESSAGE(CM_UP_CONDITIONITEM, &AddedConditionView::MsgUpConditionItem)
	ON_MESSAGE(CM_DOWN_CONDITIONITEM, &AddedConditionView::MsgDownConditionItem)
END_MESSAGE_MAP()

#ifdef _DEBUG
void AddedConditionView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void AddedConditionView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void AddedConditionView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	sizeTotal.cx = 870;
	sizeTotal.cy = ITEM_GAP;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

BOOL AddedConditionView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void AddedConditionView::Add(const std::string& index, const CString& completeText)
{
	sizeTotal.cy += ITEM_GAP;
	SetScrollSizes(MM_TEXT, sizeTotal);

	ConditionItemDlg* addedConditionDlg = new ConditionItemDlg();
	addedConditionDlg->Create(IDD_CONDITIONITEM, this);
	addedConditionDlg->ModifyStyle(0, WS_CHILD | WS_VISIBLE);

	addedConditionDlg->SetWindowPos(NULL, 0, GetTopPosition(), 0, 0, SWP_NOSIZE);
	addedConditionDlg->ShowWindow(SW_HIDE);
	addedConditionDlg->SetConditionText(completeText);
	addedConditionDlg->SetConditionIndex(index);
	addedConditionDlg->ShowWindow(SW_SHOW);

	addedConditionDlgMap[index] = addedConditionDlg;
	addedConditionDlgVec.push_back(std::make_pair(index, addedConditionDlg));
}

int AddedConditionView::GetTopPosition()
{
	return count++ * ITEM_GAP - GetScrollPosition().y;
}