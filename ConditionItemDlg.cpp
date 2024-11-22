// ConditionItemDlg.cpp: 구현 파일
//

#include "pch.h"
#include "ConditionSearch.h"
#include "afxdialogex.h"
#include "ConditionItemDlg.h"


// ConditionItemDlg 대화 상자

IMPLEMENT_DYNAMIC(ConditionItemDlg, CDialogEx)

ConditionItemDlg::ConditionItemDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONDITIONITEM, pParent)
{

}

ConditionItemDlg::~ConditionItemDlg()
{
}

void ConditionItemDlg::SetConditionIndex(const char index)
{
	conditionIndex = index;
	std::string str;
	str += index;
	SetDlgItemText(IDC_CHECK_INDEX, (LPCTSTR)str.c_str());
}

void ConditionItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_DELETE, m_Btn_Delete);
	DDX_Control(pDX, IDC_STATIC_COMPLETE_TEXT, m_StaticCompleteText);
	DDX_Control(pDX, IDC_CHECK_FORMULA, m_Check_Index);
}


BEGIN_MESSAGE_MAP(ConditionItemDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_DELETE, &ConditionItemDlg::OnBnClickedBtnDelete)
	ON_STN_CLICKED(IDC_STATIC_COMPLETE_TEXT, &ConditionItemDlg::OnStnClickedStaticCompleteText)
END_MESSAGE_MAP()

// ConditionItemDlg 메시지 처리기

void ConditionItemDlg::OnBnClickedBtnDelete()
{
	GetParent()->SendMessage(CM_DELETE_CONDITIONITEM);
}

void ConditionItemDlg::SetConditionText(const CString& completeText)
{
	SetDlgItemText(IDC_STATIC_COMPLETE_TEXT, completeText);
}

std::string ConditionItemDlg::GetCompleteText()
{
	CString cStr;
	GetDlgItemText(IDC_STATIC_COMPLETE_TEXT, cStr);
	return std::string(CT2CA(cStr));
}


void ConditionItemDlg::OnStnClickedStaticCompleteText()
{
	
}
