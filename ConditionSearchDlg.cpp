
// ConditionSearchDlg.cpp: 구현 파일

#include "pch.h"
#include "ConditionSearch.h"
#include "ConditionSearchDlg.h"
#include "afxdialogex.h"
#include "ConditionItemDlg.h"
#include "AddedConditionView.h"
#include <algorithm>

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
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_CONDITIONLIST, &CConditionSearchDlg::OnNMDblclkTreeConditionlist)
	ON_MESSAGE(CM_CLICK_CONDITIONITEM, &CConditionSearchDlg::MsgClickConditionItem)
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
//	ON_WM_RBUTTONDOWN()
ON_WM_LBUTTONDOWN()
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

	jsonParserPtr = std::make_unique<JsonParser>();
	std::vector<std::string> addedConditionIndexes;
	jsonParserPtr->LoadStatus(currentBgColor, currentFontColor, addedConditionIndexes, addedConditionInfoMap, addedConditionIndex);

	colorChangeDlg = ::CreateDlg();
	colorChangeDlg->SetDlgVariable(this, fontColorControls, &currentBgColor, &currentFontColor);
	colorChangeDlg->SetPreviewDlgVariable(previewDlg, previewDlg->GetFontColorControls(), previewDlg->GetBgColor(), previewDlg->GetFontColor());

	addedConditionView = new AddedConditionView();
	CRect viewRect{ LEFT_START, 50, LEFT_START + 900, 260 };
	addedConditionView->Create(NULL, NULL, WS_CHILD | WS_VSCROLL, viewRect, this, IDD_FORMVIEW, NULL);
	addedConditionView->OnInitialUpdate();
	addedConditionView->ShowWindow(SW_SHOW);

	// 배경 색 설정
	SetBackgroundColor(currentBgColor);

	// 폰트 색 설정
	for (auto& control : fontColorControls)
	{
		control->Invalidate();
		control->UpdateWindow();
	}

	// 종목 다이얼로그 생성
	for (const std::string& index : addedConditionIndexes)
	{
		if (addedConditionInfoMap.find(index) == addedConditionInfoMap.end())
			continue;
		const CString& completeText = addedConditionInfoMap[index]->completeTextCStr;
		addedConditionView->Add(index, completeText);
	}

	// 종목 컨트롤 생성 및 값 설정
	for (auto& p : addedConditionInfoMap)
	{
		const auto& controlInfo = xmlParserPtr->GetControlInfo(p.second->addedConditionId);
		p.second->currentControls = CreateControls(controlInfo, false);
		SaveCtrlValue(p.second->currentCtrlValues, p.second->savedControlValues);
		currentControls.clear();
	}

	GetMenu()->EnableMenuItem(ID_COND_LOAD, MF_BYCOMMAND | MF_ENABLED);

	ChangeButtonState();

	for (char a = 'A'; a <= 'Z'; a++)
		aToZ += a;

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

LRESULT CConditionSearchDlg::MsgClickConditionItem(WPARAM wParam, LPARAM lParam)
{
	const std::string index((char*)wParam);
	selectedConditionIndex = index;

	ClearCurrentControls();
	ShowCurrentControls(index);

	ChangeButtonState();

	return LRESULT();
}

LRESULT CConditionSearchDlg::MsgDeleteConditionItem(WPARAM wParam, LPARAM lParam)
{
	const std::string index((char*)wParam);
	if (addedConditionInfoMap.find(index) == addedConditionInfoMap.end())
		return LRESULT();

	for (auto& control : addedConditionInfoMap[index]->currentControls)
	{
		if (control.first != nullptr)
		{
			control.first->ShowWindow(SW_HIDE);
			control.first->DestroyWindow();
			delete control.first;
		}
	}

	addedConditionInfoMap.erase(index);
	std::cout << "delete condition item\n";

	ClearCurrentControls();

	ChangeButtonState();

	return LRESULT();
}

void CConditionSearchDlg::ClearCurrentControls()
{
	if (currentAddedControls)
	{
		for (auto& control : *currentAddedControls)
		{
			control.first->ShowWindow(SW_HIDE);
		}
		currentAddedControls = nullptr;
	}
	else
	{
		InitializeControls(currentControls);
	}
}

void CConditionSearchDlg::ShowCurrentControls(const std::string& index)
{
	if (addedConditionInfoMap.find(index) == addedConditionInfoMap.end())
		return;

	auto& condInfo = addedConditionInfoMap[index];
	for (auto& control : condInfo->currentControls)
	{
		control.first->ShowWindow(SW_SHOW);
	}
	currentAddedControls = &condInfo->currentControls;
}

CString CConditionSearchDlg::GetItemText()
{
	HTREEITEM hSelectedItem = m_TreeCtrl.GetSelectedItem();
	if (hSelectedItem != nullptr)
		return m_TreeCtrl.GetItemText(hSelectedItem);
	else
		return CString();
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

		ChangeButtonState();
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

std::vector<std::pair<CWnd*, ControlAttribute>> CConditionSearchDlg::CreateControls(const std::vector<ControlAttribute>& controlInfo, const bool bVisible)
{
	currentControls.clear();

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
			cStatic->Create(caption, WS_VISIBLE | SS_LEFT | SS_LEFTNOWORDWRAP, rect, this, ID_START + att.id + idCount);

			wnd = cStatic;
		}
		else if (att.controlType == EControlType::ECT_Edit)
		{
			CEdit* edit = new CEdit();
			edit->Create(WS_VISIBLE | SS_LEFT, rect, this, ID_START + att.id + idCount++);
			CString defaultValue = MultibyteToUnicode(att.defaultValue.c_str()).c_str();
			edit->SetWindowText(defaultValue);
			
			wnd = edit;
		}
		else if (att.controlType == EControlType::ECT_Radio)
		{
			CButton* button = new CButton();
			CString caption = MultibyteToUnicode(att.staticCaption.c_str()).c_str();
			button->Create(caption, WS_VISIBLE | BS_AUTORADIOBUTTON, rect, this, ID_START + att.id + idCount);
			
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
			comboBox->Create(WS_VISIBLE | CBS_DROPDOWN, rect, this, ID_START + att.id + idCount);
			for (const std::string& item : att.comboItem)
				comboBox->AddString(MultibyteToUnicode(item.c_str()).c_str());
			comboBox->SetCurSel(0);

			wnd = comboBox;
		}

		if (bVisible == false)
			wnd->ShowWindow(SW_HIDE);

		idCount += 50;

		SetControlFontSize(this, wnd, 8);
		currentControls.emplace_back(std::make_pair(wnd, att));
	}

	return currentControls;
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

	selectedConditionIndex = addedConditionIndex;

	std::shared_ptr<AddedConditionInfo> addedCondInfoPtr = std::make_shared<AddedConditionInfo>();
	addedConditionInfoMap[addedConditionIndex] = addedCondInfoPtr;

	addedCondInfoPtr->addedConditionId = xmlParserPtr->GetConditionId(GetItemText());

	addedCondInfoPtr->currentControls = currentControls;
	currentAddedControls = &addedCondInfoPtr->currentControls;
	MakeCompleteText(addedCondInfoPtr->completeText, addedCondInfoPtr->completeTextCStr);
	SaveCtrlValue(addedCondInfoPtr->currentCtrlValues, addedCondInfoPtr->savedControlValues);

	addedConditionView->Add(addedConditionIndex, addedCondInfoPtr->completeTextCStr);

	PlusAddedConditionIndex();

	currentControls.clear();

	ChangeButtonState();
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
	if (!CheckIsCondition())
		return;

	std::vector<std::string>& completeIdxName = GetCompleteIdxName();
	if (completeIdxName.empty())
		return;

	if (completeIdxName.size() == 1)
	{
		completeText = completeIdxName[0];
		ApplyPacketIdReplacements(completeText);
	}
	else
	{
		std::string groupId = GetSelectedGroupId();
		assert(!groupId.empty());

		int idx = GetGroupIndex(groupId);
		assert(idx < completeIdxName.size());
		completeText = completeIdxName[idx];

		ApplyPacketIdReplacements(completeText, groupId);
	}

	completeTextCStr = MultibyteToUnicode(completeText.c_str()).c_str();
}

std::vector<std::string> CConditionSearchDlg::GetCompleteIdxName()
{
	if (currentAddedControls)
	{
		if (addedConditionInfoMap.find(selectedConditionIndex) == addedConditionInfoMap.end())
			return std::vector<std::string>();

		const std::string& conditionId = addedConditionInfoMap[selectedConditionIndex]->addedConditionId;
		return xmlParserPtr->GetCompleteIndexName(conditionId);
	}
	else
	{
		CString itemText = GetItemText();
		return xmlParserPtr->GetCompleteIndexName(itemText);
	}
}

std::string CConditionSearchDlg::GetSelectedGroupId()
{
	auto& controls = currentAddedControls ? *currentAddedControls : currentControls;

	for (auto& pair : controls)
	{
		if (pair.second.controlType == EControlType::ECT_Radio)
		{
			CButton* radioBtn = dynamic_cast<CButton*>(pair.first);
			assert(radioBtn);

			if (radioBtn->GetCheck())
				return pair.second.radioGroupId;
		}
	}

	return {};
}

int CConditionSearchDlg::GetGroupIndex(const std::string& groupId)
{
	size_t underscorePos = groupId.find("_");
	assert(underscorePos != std::string::npos);

	return stoi(groupId.substr(underscorePos + 1)) - 1;
}

void CConditionSearchDlg::ApplyPacketIdReplacements(std::string& completeText, const std::string& groupId)
{
	auto& controls = currentAddedControls ? *currentAddedControls : currentControls;

	for (auto& pair : controls)
	{
		if (groupId.empty() || (pair.second.packetId[0] == 'P' || pair.second.radioGroupId == groupId))
		{
			ReplacePakcetIdToValue(pair.second.controlType, pair.first->GetDlgCtrlID(), pair.second.packetId, completeText);
		}
	}
}

void CConditionSearchDlg::ApplyPacketIdReplacements(std::string& completeText)
{
	ApplyPacketIdReplacements(completeText, {});
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
	
	auto& addedCondInfoPtr = addedConditionInfoMap[selectedConditionIndex];

	MakeCompleteText(addedCondInfoPtr->completeText, addedCondInfoPtr->completeTextCStr);
	SaveCtrlValue(addedCondInfoPtr->currentCtrlValues, addedCondInfoPtr->savedControlValues);
	addedConditionView->SetConditionText(selectedConditionIndex, addedCondInfoPtr->completeTextCStr);
}


void CConditionSearchDlg::OnBtnClickedBtnSearch()
{
	// 리턴 조건 추가
	
	m_ListCtrl.DeleteAllItems();
	
	for (char key : aToZ)
	{
		std::string keyStr = std::string(1, key);
		if (addedConditionInfoMap.find(keyStr) == addedConditionInfoMap.end())
			continue;

		bool bValidKey = csvParserPtr->SearchData(&m_ListCtrl, addedConditionInfoMap[keyStr]->currentCtrlValues, addedConditionInfoMap[keyStr]->addedConditionId);
		if (bValidKey)
			break;
	}
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
	if (dlg.DoModal() == IDOK)
	{
		const std::string pathName = std::string(CT2CA(dlg.GetPathName()));
		auto& indexOrder = addedConditionView->GetIndexOrder();
		xmlParserPtr->SaveConditionInfoToXML(pathName, indexOrder, addedConditionInfoMap);
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
		InitializeConditionsAndControls();

		addedConditionView->ClearConditionDlgs();

		const std::wstring pathName = dlg.GetPathName();
		std::vector<std::string> indexOrder;
		xmlParserPtr->LoadConditionInfo(pathName, indexOrder, addedConditionInfoMap, addedConditionIndex);

		// 종목 다이얼로그 생성
		for (const std::string& index : indexOrder)
		{
			if (addedConditionInfoMap.find(index) == addedConditionInfoMap.end())
				continue;
			const CString& completeText = addedConditionInfoMap[index]->completeTextCStr;
			addedConditionView->Add(index, completeText);
		}

		// 종목 컨트롤 생성 및 값 설정
		for (auto& p : addedConditionInfoMap)
		{
			const auto& controlInfo = xmlParserPtr->GetControlInfo(p.second->addedConditionId);
			p.second->currentControls = CreateControls(controlInfo, false);
			SaveCtrlValue(p.second->currentCtrlValues, p.second->savedControlValues);
			currentControls.clear();
		}

		ChangeButtonState();
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

void CConditionSearchDlg::InitializeConditionsAndControls()
{
	InitializeControls(currentControls);
	currentAddedControls = nullptr;
	for (auto& p : addedConditionInfoMap)
	{
		InitializeControls(p.second->currentControls);
	}
	addedConditionInfoMap.clear();
	addedConditionIndex = "";
	selectedConditionIndex = "";
}

void CConditionSearchDlg::InitializeControls(std::vector<std::pair<CWnd*, ControlAttribute>>& controls)
{
	for (auto& control : controls)
	{
		if (control.first != nullptr)
		{
			control.first->ShowWindow(SW_HIDE);
			control.first->DestroyWindow();
			delete control.first;
		}
	}
	controls.clear();
}

void CConditionSearchDlg::ChangeButtonState()
{
	if (addedConditionInfoMap.size())
	{
		GetMenu()->EnableMenuItem(ID_COND_SAVE, MF_BYCOMMAND | MF_ENABLED);
		m_Btn_Search.EnableWindow(true);
	}
	else
	{
		GetMenu()->EnableMenuItem(ID_COND_SAVE, MF_BYCOMMAND | MF_DISABLED);
		m_Btn_Search.EnableWindow(false);
	}

	if (currentAddedControls)
		m_BtnModify.EnableWindow(true);
	else
		m_BtnModify.EnableWindow(false);

	if(currentControls.size())
		m_BtnAdd.EnableWindow(true);
	else
		m_BtnAdd.EnableWindow(false);

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
			auto& indexOrder = addedConditionView->GetIndexOrder();
			jsonParserPtr->SaveStatusToJson(currentBgColor, currentFontColor, indexOrder, addedConditionInfoMap);
		}

		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CConditionSearchDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	

	CDialogEx::OnLButtonDown(nFlags, point);
}
