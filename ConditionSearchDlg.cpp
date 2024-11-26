
// ConditionSearchDlg.cpp: 구현 파일

#include "pch.h"
#include "ConditionSearch.h"
#include "ConditionSearchDlg.h"
#include "afxdialogex.h"
#include "ConditionItemDlg.h"
#include "AddedConditionView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LEFT_START 330
#define TOP_START 200

#define CONDITION_ITEM_Y 30

#define ID_START 10500

CConditionSearchDlg::CConditionSearchDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONDITIONSEARCH_DIALOG, pParent), addedConditionIndex("A")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConditionSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_CONDITIONLIST, m_TreeCtrl);
	DDX_Control(pDX, IDC_BTN_MODIFY, m_BtnModify);
	DDX_Control(pDX, IDC_BTN_ADD, m_BtnAdd);
	DDX_Control(pDX, IDC_LIST_RESULT, m_ListCtrl);
	DDX_Control(pDX, IDC_BTN_SEARCH, m_Btn_Search);
}

BEGIN_MESSAGE_MAP(CConditionSearchDlg, CDialogEx)
//	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_CONDITIONLIST, &CConditionSearchDlg::OnTvnSelchangedTreeConditionlist)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_CONDITIONLIST, &CConditionSearchDlg::OnNMDblclkTreeConditionlist)
	ON_MESSAGE(CM_DELETE_CONDITIONITEM, &CConditionSearchDlg::MsgDeleteConditionItem)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_ADD, &CConditionSearchDlg::OnBtnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_MODIFY, &CConditionSearchDlg::OnBtnClickedBtnModify)
	ON_BN_CLICKED(IDC_BTN_SEARCH, &CConditionSearchDlg::OnBtnClickedBtnSearch)
	ON_COMMAND(ID_COND_SAVE, &CConditionSearchDlg::OnCondSave)
	ON_COMMAND(ID_COND_LOAD, &CConditionSearchDlg::OnCondLoad)
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_SET_COLOR, &CConditionSearchDlg::OnSetColor)
	ON_WM_CTLCOLOR()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()

// CConditionSearchDlg 메시지 처리기

BOOL CConditionSearchDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	fontColorControls.push_back(GetDlgItem(IDC_GROUPBOX_CONDLIST));
	fontColorControls.push_back(GetDlgItem(IDC_GROUPBOX_CONDLIST2));
	fontColorControls.push_back(GetDlgItem(IDC_GROUPBOX_CONDSETTING));
	fontColorControls.push_back(GetDlgItem(IDC_GROUPBOX_SEARCHRESULT));
	fontColorControls.push_back(GetDlgItem(IDC_BTN_SEARCH));
	fontColorControls.push_back(GetDlgItem(IDC_BTN_MODIFY));
	fontColorControls.push_back(GetDlgItem(IDC_BTN_ADD));
	fontColorControls.push_back(GetDlgItem(IDC_TREE_CONDITIONLIST));

	SetWindowTheme(GetDlgItem(IDC_GROUPBOX_CONDLIST)->m_hWnd, _T(""), _T(""));
	SetWindowTheme(GetDlgItem(IDC_GROUPBOX_CONDLIST2)->m_hWnd, _T(""), _T(""));
	SetWindowTheme(GetDlgItem(IDC_GROUPBOX_CONDSETTING)->m_hWnd, _T(""), _T(""));
	SetWindowTheme(GetDlgItem(IDC_GROUPBOX_SEARCHRESULT)->m_hWnd, _T(""), _T(""));

	currentBgColor = RGB(240, 240, 240);

	m_BtnAdd.EnableWindow(false);
	m_BtnModify.EnableWindow(false);

	GetMenu()->EnableMenuItem(ID_COND_SAVE, MF_BYCOMMAND | MF_DISABLED);

	if (!bPreviewMode)
	{
		previewDlg = new CConditionSearchDlg();
		previewDlg->SetToPreviewMode();
		previewDlg->Create(IDD_CONDITIONSEARCH_DIALOG, this);
		previewDlg->SetWindowText(L"미리보기");
	}
	else
	{
		ShowWindow(SW_SHOWMINIMIZED);
		PostMessage(WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);
		return TRUE;
	}

	CRect rect;
	GetDlgItem(IDC_GROUPBOX_CONDSETTING)->GetWindowRect(rect);
	leftStart = rect.left - 20, topStart = rect.top - 30;

	GetClientRect(&m_rcMinimumDialog);
	CalcWindowRect(m_rcMinimumDialog);

	xmlParserPtr = std::make_unique<XMLParser>();
	if (xmlParserPtr)
	{
		if (xmlParserPtr->InsertTreeCommonData(&m_TreeCtrl) == false)
		{
			// error
		}
		else
		{
			xmlParserPtr->ReadMapFile();
		}
	}

	csvParserPtr = std::make_unique<CSVParser>();
	if (csvParserPtr)
	{
		if (csvParserPtr->ParsingCSV(&m_ListCtrl) == false)
		{
			// error
		}
	}

	//jsonParserPtr = std::make_unique<JsonParser>();
	//jsonParserPtr->LoadStatus(currentBgColor, currentFontColor, addedConditionId, completeText, savedControlValues);

	// 배경 색 설정
	SetBackgroundColor(currentBgColor);

	// 폰트 색 설정
	for (auto& control : fontColorControls)
	{
		control->Invalidate();
		control->UpdateWindow();
	}
	
	//if (addedConditionId.length() > 0)
	//{
	//	// 조건 컨트롤 생성 및 값 설정
	//	LoadControlStatus();

	//	m_BtnModify.EnableWindow(true);

	//	// 컨디션 dlg 생성 및 텍스트 설정
	//	completeTextCStr = MultibyteToUnicode(completeText.c_str()).c_str();
	//	CreateConditionDialog();
	//}

	colorChangeDlg = ::CreateDlg();
	colorChangeDlg->SetDlgVariable(this, fontColorControls, &currentBgColor, &currentFontColor);
	colorChangeDlg->SetPreviewDlgVariable(previewDlg, previewDlg->GetFontColorControls(), previewDlg->GetBgColor(), previewDlg->GetFontColor());

	addedConditionView = new AddedConditionView();
	CRect viewRect{ LEFT_START, 50, LEFT_START + 900, 260 };
	addedConditionView->Create(NULL, NULL, WS_CHILD | WS_VSCROLL, viewRect, this, IDD_FORMVIEW, NULL);
	addedConditionView->OnInitialUpdate();
	addedConditionView->ShowWindow(SW_SHOW);

	return TRUE;
}

void CConditionSearchDlg::LoadControlStatus()
{
	/*const auto& controlInfo = xmlParserPtr->GetControlInfo(addedConditionId);
	CreateControls(controlInfo);

	for (auto& pair : savedControlValues)
	{
		for (auto& control : currentControls)
		{
			if (pair.first == control.second.id)
			{
				control.first->SetWindowTextW(MultibyteToUnicode(pair.second.c_str()).c_str());

				if (control.second.controlType == EControlType::ECT_Radio)
				{
					CButton* btn = dynamic_cast<CButton*>(control.first);
					btn->SetCheck(true);
				}
			}
		}
	}*/
}

void CConditionSearchDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CConditionSearchDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CConditionSearchDlg::MsgDeleteConditionItem(WPARAM wParam, LPARAM lParam)
{
	const std::string index((char*)wParam);
	if (addedConditionInfoMap.find(index) == addedConditionInfoMap.end())
		return LRESULT();

	addedConditionInfoMap.erase(index);
	std::cout << "delete condition item\n";

	/*ClearCurrentControls();

	GetMenu()->EnableMenuItem(ID_COND_SAVE, MF_BYCOMMAND | MF_DISABLED);
	GetMenu()->EnableMenuItem(ID_COND_LOAD, MF_BYCOMMAND | MF_ENABLED);*/

	return LRESULT();
}

void CConditionSearchDlg::ClearCurrentControls()
{
	for (auto& control : currentControls)
	{
		if (control.first != nullptr)
			delete control.first;
	}
	currentControls.clear();
}

CString CConditionSearchDlg::GetItemText()
{
	HTREEITEM hSelectedItem = m_TreeCtrl.GetSelectedItem();
	if (hSelectedItem != nullptr)
		return m_TreeCtrl.GetItemText(hSelectedItem);
	else
		return CString();
}

void CConditionSearchDlg::OnTvnSelchangedTreeConditionlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CConditionSearchDlg::OnNMDblclkTreeConditionlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (bPreviewMode)
		return;

	if (CheckIsCondition())
	{
		ClearCurrentControls();
		
		// 세부 설정 창 생성
		const auto& controlInfo = xmlParserPtr->GetControlInfo(GetItemText());
		CreateControls(controlInfo);
		m_BtnAdd.EnableWindow(true);
	}
}

bool CConditionSearchDlg::CheckIsCondition()
{
	HTREEITEM hSelectedItem = m_TreeCtrl.GetSelectedItem();
	if (hSelectedItem != nullptr)
	{
		CString itemText = m_TreeCtrl.GetItemText(hSelectedItem);
		if (xmlParserPtr->IsCondition(itemText))
		{
			return true;
		}
	}
	return false;
}

void CConditionSearchDlg::CreateControls(const std::vector<ControlAttribute>& controlInfo)
{
	bool firstRadio = true;

	std::unordered_map<int, std::vector<int>> topMap;

	for (const auto& att : controlInfo)
	{
		CWnd* wnd = nullptr;

		const float posTop = topStart + (att.topLine - 1) * 40.f;

		int posLeft = 0;
		if (topMap.find(posTop) != topMap.end())
			posLeft = topMap[posTop].back() + 10;
		else
			posLeft = leftStart + att.left + 10;

		const int posLeftEnd = posLeft + att.width + 15;
		topMap[posTop].push_back(posLeftEnd);

		CRect rect = CRect(posLeft, posTop, posLeftEnd, posTop + att.height + 5);

		if (att.controlType == EControlType::ECT_Static)
		{
			CStatic* cStatic = new CStatic();
			
			CString caption = MultibyteToUnicode(att.staticCaption.c_str()).c_str();
			cStatic->Create(caption, WS_VISIBLE | SS_LEFT | SS_LEFTNOWORDWRAP, rect, this, ID_START + att.id);

			wnd = cStatic;
		}
		else if (att.controlType == EControlType::ECT_Edit)
		{
			CEdit* edit = new CEdit();
			edit->Create(WS_VISIBLE | SS_LEFT, rect, this, ID_START + att.id);
			CString defaultValue = MultibyteToUnicode(att.defaultValue.c_str()).c_str();
			edit->SetWindowText(defaultValue);
			
			wnd = edit;
		}
		else if (att.controlType == EControlType::ECT_Radio)
		{
			CButton* button = new CButton();
			CString caption = MultibyteToUnicode(att.staticCaption.c_str()).c_str();
			button->Create(caption, WS_VISIBLE | BS_AUTORADIOBUTTON, rect, this, ID_START + att.id);
			
			if (firstRadio)
			{
				button->SetCheck(BST_CHECKED);
				firstRadio = false;
			}

			wnd = button;
		}
		else if (att.controlType == EControlType::ECT_ComboBox)
		{
			CComboBox* comboBox = new CComboBox();
			comboBox->Create(WS_VISIBLE | CBS_DROPDOWN, rect, this, ID_START + att.id);
			for (const std::string& item : att.comboItem)
				comboBox->AddString(MultibyteToUnicode(item.c_str()).c_str());
			comboBox->SetCurSel(0);

			wnd = comboBox;
		}

		SetControlFontSize(this, wnd, 8);
		currentControls.emplace_back(std::make_pair(wnd, att));
	}
}

void CConditionSearchDlg::SetControlFontSize(CWnd* pParentWnd, CWnd* pControl, int fontSize)
{
	CFont* pFont = new CFont();

	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	lf.lfHeight = -MulDiv(fontSize, GetDeviceCaps(pParentWnd->GetDC()->m_hDC, LOGPIXELSY), 72);
	//_tcscpy_s(lf.lfFaceName, _T("Microsoft Sans Serif"));

	pFont->CreateFontIndirect(&lf);

	pControl->SetFont(pFont);
	pControl->SetFont(pFont, TRUE);
}

void CConditionSearchDlg::OnBtnClickedBtnAdd()
{
	if (CheckIsCondition() == false)
		return;

	if (IsAbleToAddCondition() == false)
		return;

	std::shared_ptr<AddedConditionInfo> addedCondInfoPtr = std::make_shared<AddedConditionInfo>();
	
	addedCondInfoPtr->addedConditionId = xmlParserPtr->GetConditionId(GetItemText());

	MakeCompleteText(addedCondInfoPtr->completeText, addedCondInfoPtr->completeTextCStr);
	SaveCtrlValue(addedCondInfoPtr->currentCtrlValues, addedCondInfoPtr->savedControlValues);

	addedConditionInfoMap[addedConditionIndex] = addedCondInfoPtr;

	addedConditionView->Add(addedConditionIndex, addedCondInfoPtr->completeTextCStr);
	PlusAddedConditionIndex();

	m_BtnModify.EnableWindow(true);
	m_BtnAdd.EnableWindow(false);

	GetMenu()->EnableMenuItem(ID_COND_SAVE, MF_BYCOMMAND | MF_ENABLED);
	GetMenu()->EnableMenuItem(ID_COND_LOAD, MF_BYCOMMAND | MF_DISABLED);
}

bool CConditionSearchDlg::IsAbleToAddCondition()
{
	if (addedConditionInfoMap.size() < MAX_CONDITION)
	{
		return true;
	}
	else
	{
		AfxMessageBox(L"조건 추가는 10개 까지 가능합니다.");
		return false;
	}
}

void CConditionSearchDlg::MakeCompleteText(std::string& completeText, CString& completeTextCStr)
{
	if (CheckIsCondition() == false)
		return;

	CString itemText = GetItemText();
	const auto& completeIdxName = xmlParserPtr->GetCompleteIndexName(itemText);

	if (completeIdxName.size() == 0)
		return;

	if (completeIdxName.size() == 1)
	{
		completeText = completeIdxName[0];

		for (auto& pair : currentControls)
		{
			ReplacePakcetIdToValue(pair.second.controlType, pair.first->GetDlgCtrlID(), pair.second.packetId, completeText);
		}
	}
	else
	{
		std::string groupId;

		for (auto& pair : currentControls)
		{
			if (pair.second.controlType == EControlType::ECT_Radio)
			{
				CButton* radioBtn = dynamic_cast<CButton*>(pair.first);
				assert(radioBtn);

				if (radioBtn->GetCheck())
				{
					groupId = pair.second.radioGroupId;
					break;
				}
			}
		}

		const int idx = stoi(groupId.substr(groupId.find("_") + 1)) - 1;
		assert(idx < completeIdxName.size());
		completeText = completeIdxName[idx];
		
		for (auto& pair : currentControls)
		{
			if (pair.second.packetId[0] != 'P' && pair.second.radioGroupId != groupId)
				continue;
		
			ReplacePakcetIdToValue(pair.second.controlType, pair.first->GetDlgCtrlID(), pair.second.packetId, completeText);
		}
	}

	completeTextCStr = MultibyteToUnicode(completeText.c_str()).c_str();
}

void CConditionSearchDlg::ReplacePakcetIdToValue(const EControlType controlType, const int ctrlId, const std::string& packetId, std::string& completeText)
{
	if (controlType == EControlType::ECT_Static)
		return;

	CString ctrlText;
	GetDlgItemText(ctrlId, ctrlText);

	std::string bracedPacketId = "(" + packetId + ")";
	std::string newText = std::string(CT2CA(ctrlText));

	if (newText.length() == 0)
		return;

	if(completeText.find(bracedPacketId) != std::string::npos)
		completeText.replace(completeText.find(bracedPacketId), bracedPacketId.length(), newText);
}

ConditionItemDlg* CConditionSearchDlg::CreateConditionDialog(const CString& completeTextCStr)
{
	ConditionItemDlg* addedConditionDlg = new ConditionItemDlg();
	addedConditionDlg->Create(IDD_CONDITIONITEM, addedConditionView);
	addedConditionDlg->ShowWindow(SW_HIDE);
	addedConditionDlg->SetWindowPos(NULL, 330, GetConditionDlgY(), 0, 0, SWP_NOSIZE);

	// 종목검색조건 다이얼로그에 표시될 텍스트 설정
	addedConditionDlg->SetConditionText(completeTextCStr);
	addedConditionDlg->ShowWindow(SW_SHOW);

	return addedConditionDlg;
}

int CConditionSearchDlg::GetConditionDlgY()
{
	return (addedConditionDlgMap.size() + 1) * 50;
}

void CConditionSearchDlg::SaveCtrlValue(std::vector<std::string>& currentCtrlValues, std::vector<std::pair<int, std::string>>& savedCtrlValues)
{
	std::string groupId = "";
	for (auto& pair : currentControls)
	{
		auto ctrlType = pair.second.controlType;
		if (ctrlType == EControlType::ECT_Radio)
		{
			auto radioBtn = dynamic_cast<CButton*>(pair.first);
			assert(radioBtn);

			if (radioBtn->GetCheck())
			{
				groupId = pair.second.radioGroupId;
			}
		}
	}

	for (auto& pair : currentControls)
	{
		auto ctrlType = pair.second.controlType;
		if (ctrlType == EControlType::ECT_Static || ctrlType == EControlType::ECT_None)
			continue;

		CString ctrlText;
		GetDlgItemText(pair.first->GetDlgCtrlID(), ctrlText);

		currentCtrlValues.push_back(std::string(CT2CA(ctrlText)));

		if (groupId.length() == 0 || (groupId.length() > 0 && groupId == pair.second.radioGroupId))
		{
			savedCtrlValues.push_back(std::make_pair(pair.second.id, std::string(CT2CA(ctrlText))));
		}
	}
}

void CConditionSearchDlg::PlusAddedConditionIndex()
{
	addedConditionIndex[0]++;
}

void CConditionSearchDlg::OnBtnClickedBtnModify()
{
	assert(addedConditionInfoMap.find(selectedConditionIndex) != addedConditionInfoMap.end());
	assert(addedConditionDlgMap.find(selectedConditionIndex) != addedConditionDlgMap.end());
	
	auto& addedCondInfoPtr = addedConditionInfoMap[selectedConditionIndex];

	MakeCompleteText(addedCondInfoPtr->completeText, addedCondInfoPtr->completeTextCStr);
	SaveCtrlValue(addedCondInfoPtr->currentCtrlValues, addedCondInfoPtr->savedControlValues);
	addedConditionDlgMap[selectedConditionIndex]->SetConditionText(addedCondInfoPtr->completeTextCStr);
}


void CConditionSearchDlg::OnBtnClickedBtnSearch()
{
if (addedConditionDlgMap.size() == 0 || bPreviewMode)
		return;
	
	m_ListCtrl.DeleteAllItems();
	csvParserPtr->SearchData(&m_ListCtrl, addedConditionInfoMap["A"]->currentCtrlValues, addedConditionInfoMap["A"]->addedConditionId);
}

void CConditionSearchDlg::ClearListControl()
{
	const int itemCount = m_ListCtrl.GetItemCount();
	for (int i = 1; i < itemCount; i++)
	{
		m_ListCtrl.DeleteItem(0);
	}
}

void CConditionSearchDlg::OnCondSave()
{
	if (GetFileAttributes(conditionSavePath) == INVALID_FILE_ATTRIBUTES)
	{
		if (CreateDirectoryW(conditionSavePath, NULL) == false)
			return;
	}

	CString fileFilter = _T("XML Files (*.xml)|*.xml|");
	CString defaultFilter = _T("xml");
	CFileDialog dlg(FALSE, defaultFilter, NULL, OFN_OVERWRITEPROMPT, fileFilter);
	if (IDOK == dlg.DoModal())
	{
		/*const std::string pathName = std::string(CT2CA(dlg.GetPathName()));

		const std::string completeText = m_ConditionItem->GetCompleteText();
		const std::string filePathStr = std::string(CT2CA(conditionSavePath)) + "//" + completeText + ".xml";

		xmlParserPtr->SaveConditionInfoToXML(pathName, addedConditionId, completeText, savedControlValues);*/
	}
}

void CConditionSearchDlg::OnCondLoad()
{
	if (bPreviewMode)
		return;

	CString fileFilter = _T("XML Files (*.xml)|*.xml|");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, fileFilter);
	if (IDOK == dlg.DoModal())
	{
		//const std::string pathName = std::string(CT2CA(dlg.GetPathName()));
		//std::wstring pathName2 = dlg.GetPathName();
		//xmlParserPtr->LoadConditionInfo(pathName2, addedConditionId, completeText, savedControlValues);
		//completeTextCStr = MultibyteToUnicode(completeText.c_str()).c_str();

		//// 종목 다이얼로그 텍스트 설정
		//CreateConditionDialog();

		//// 조건 세부 설정 컨트롤 생성 및 값 설정
		//LoadControlStatus();

		//m_BtnModify.EnableWindow(true);

		//GetMenu()->EnableMenuItem(ID_COND_SAVE, MF_BYCOMMAND | MF_ENABLED);
		//GetMenu()->EnableMenuItem(ID_COND_LOAD, MF_BYCOMMAND | MF_DISABLED);
	}
}

void CConditionSearchDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_rcMinimumDialog.Width();
	lpMMI->ptMinTrackSize.y = m_rcMinimumDialog.Height();

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CConditionSearchDlg::OnSetColor()
{
	if (bPreviewMode)
		return;

	colorChangeDlg->DoModal();
}


HBRUSH CConditionSearchDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	for (auto control : fontColorControls)
	{
		if (control->m_hWnd == pWnd->m_hWnd)
		{
			pDC->SetTextColor(currentFontColor);
		}
	}

	return hbr;
}

void CConditionSearchDlg::SetToPreviewMode()
{
	bPreviewMode = true;
}


void CConditionSearchDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (bPreviewMode)
	{
		if (nID == SC_CLOSE)
		{
			ShowWindow(SW_SHOWMINIMIZED);
			PostMessage(WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);
		}
	}
	else
	{
		if (nID == SC_CLOSE)
		{
			//jsonParserPtr->SaveStatusToJson(currentBgColor, currentFontColor, addedConditionId, completeText, savedControlValues);
		}

		CDialogEx::OnSysCommand(nID, lParam);
	}
}
