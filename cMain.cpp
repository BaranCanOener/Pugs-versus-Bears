#include "cMain.h"
#include "wx/sizer.h"
#include "resource.h"
#include "wx/mstream.h"
#include "wx/msw/private.h"
#include "string.h"
#include <thread>

char numToLetter(char coord) {
	switch (coord)
	{
	case 0:
		return 'a';
		break;
	case 1:
		return 'b';
		break;
	case 2:
		return 'c';
		break;
	case 3:
		return 'd';
		break;
	case 4:
		return 'e';
		break;
	case 5:
		return 'f';
		break;
	case 6:
		return 'g';
		break;
	case 7:
		return 'h';
		break;
	default:
		return 127;
	}
}

char letterToNum(char coord) {
	switch (coord)
	{
	case 'a':
		return 0;
		break;
	case 'b':
		return 1;
		break;
	case 'c':
		return 2;
		break;
	case 'd':
		return 3;
		break;
	case 'e':
		return 4;
		break;
	case 'f':
		return 5;
		break;
	case 'g':
		return 6;
		break;
	case 'h':
		return 7;
		break;
	default:
		return 127;
	}
}

Colour switchColour(Colour colour) {
	if (colour == Colour::White) {
		return Colour::Black;
	}
	else {
		return Colour::White;
	}
}

wxMemoryInputStream* GetResourceInputStream(int resource_ID, LPCWSTR resource_type) {
	HRSRC hrsrc = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resource_ID), resource_type);
	if (hrsrc == NULL) return NULL;

	HGLOBAL hglobal = LoadResource(wxGetInstance(), hrsrc);
	if (hglobal == NULL) return NULL;

	void* data = LockResource(hglobal);
	if (data == NULL) return NULL;

	DWORD datalen = SizeofResource(wxGetInstance(), hrsrc);
	if (datalen < 1) return NULL;

	return new wxMemoryInputStream(data, datalen);
}

BEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_SIZE(cMain::sizeEvent)
	EVT_BUTTON(10001, cMain::undoMoveClicked)
	EVT_BUTTON(10002, cMain::resetBoardClicked)
	EVT_BUTTON(10003, cMain::aiMoveClicked)
	EVT_BUTTON(10005, cMain::aiAbortClicked)
	EVT_BUTTON(10006, cMain::resetStatsClicked)
	EVT_RADIOBUTTON(10007, cMain::radioButtonClicked)
	EVT_RADIOBUTTON(10008, cMain::radioButtonClicked)
	EVT_TIMER(10004, cMain::OnTimer)
END_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Pugs versus Bears", wxPoint(30,30), wxSize(8*96+180,8*96)) {

	this->board = new ChessBoard();
	this->engine = new Engine();
	this->writePieceInfo(board, squares);

	const wxSize buttonSize = wxSize(180, 30);

	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBox* box_gameStats = new wxStaticBox(this, wxID_ANY, "Statistics", wxPoint(5, 5), wxSize(190, 80));
	text_totalGames = new wxStaticText(box_gameStats, wxID_ANY, "", wxPoint(10, 15));
	text_whiteWins = new wxStaticText(box_gameStats, wxID_ANY,  "", wxPoint(10, 30));
	text_blackWins = new wxStaticText(box_gameStats, wxID_ANY,  "", wxPoint(10, 45));
	text_draws = new wxStaticText(box_gameStats, wxID_ANY,      "", wxPoint(10, 60));
	btn_resetStats = new wxButton(box_gameStats, 10006, "Reset", wxPoint(120, 15), wxSize(65,30));

	wxStaticBox* box_gameControls = new wxStaticBox(this, wxID_ANY, "Current Game", wxPoint(5, 90), wxSize(190, 205));
	text_lastMove = new wxStaticText(box_gameControls, wxID_ANY, "", wxPoint(5, 15));
	text_turn = new wxStaticText(box_gameControls, wxID_ANY, "", wxPoint(5, 30));
	text_legalMoveCount = new wxStaticText(box_gameControls, wxID_ANY, "", wxPoint(5, 45));
	text_captureMoveCount = new wxStaticText(box_gameControls, wxID_ANY, "", wxPoint(5, 60));
	btn_undoMove = new wxButton(box_gameControls, 10001, "Undo Move", wxPoint(5, 75), buttonSize);
	btn_resetBoard = new wxButton(box_gameControls, 10002, "Reset Board", wxPoint(5, 105), buttonSize);
	btn_aiMove = new wxButton(box_gameControls, 10003, "Calculate Engine Move", wxPoint(5, 135), buttonSize);
	btn_abortAiMove = new wxButton(box_gameControls, 10005, "Stop Calculation", wxPoint(5, 165), buttonSize);

	box_whiteEngineControls = new wxStaticBox(this, wxID_ANY, "Engine (White)", wxPoint(5, 405), wxSize(190, 100));
	text_whiteAllocTime = new wxStaticText(box_whiteEngineControls, wxID_ANY, "Allocated time (ms): ", wxPoint(5, 15));
	textctrl_whiteAllocTime = new wxTextCtrl(box_whiteEngineControls, wxID_ANY, "5000", wxPoint(120, 15), wxSize(45, 20));
	text_whiteQuiescenceDepth = new wxStaticText(box_whiteEngineControls, wxID_ANY, "Quiescence depth: ", wxPoint(5, 40));
	textctrl_whiteQuiescenceDepth = new wxTextCtrl(box_whiteEngineControls, wxID_ANY, "2", wxPoint(120, 40), wxSize(45, 20));
	checkbox_whiteRandomize = new wxCheckBox(box_whiteEngineControls, wxID_ANY, "Randomize turns", wxPoint(5,65));
	checkbox_whiteAutorespond = new wxCheckBox(box_whiteEngineControls, wxID_ANY, "Autorespond", wxPoint(5,80));;

	box_blackEngineControls = new wxStaticBox(this, wxID_ANY, "Engine (Black)", wxPoint(5, 300), wxSize(190, 100));
	text_blackAllocTime = new wxStaticText(box_blackEngineControls, wxID_ANY, "Allocated time (ms): ", wxPoint(5, 15));
	textctrl_blackAllocTime = new wxTextCtrl(box_blackEngineControls, wxID_ANY, "5000", wxPoint(120, 15), wxSize(45, 20));
	text_blackQuiescenceDepth = new wxStaticText(box_blackEngineControls, wxID_ANY, "Quiescence depth: ", wxPoint(5, 40));
	textctrl_blackQuiescenceDepth = new wxTextCtrl(box_blackEngineControls, wxID_ANY, "2", wxPoint(120, 40), wxSize(45, 20));
	checkbox_blackRandomize = new wxCheckBox(box_blackEngineControls, wxID_ANY, "Randomize turns", wxPoint(5, 65));
	checkbox_blackAutorespond = new wxCheckBox(box_blackEngineControls, wxID_ANY, "Autorespond", wxPoint(5, 80));

	box_engineInfo = new wxStaticBox(this, wxID_ANY, "Engine Progress", wxPoint(5, 510), wxSize(190, 160));
	gauge_engineProgress = new wxGauge(box_engineInfo, wxID_ANY, engine->timeLimit, wxPoint(5,20), wxSize(180,20));
	text_moveData = new wxStaticText(box_engineInfo, wxID_ANY, "opt", wxPoint(5, 45));
	text_depth = new wxStaticText(box_engineInfo, wxID_ANY, "Depth", wxPoint(5, 60));
	text_timePassed = new wxStaticText(box_engineInfo, wxID_ANY, "Time, speed", wxPoint(5, 75));
	text_nodes = new wxStaticText(box_engineInfo, wxID_ANY, "Normal nodes", wxPoint(5, 90));
	text_horizonNodes = new wxStaticText(box_engineInfo, wxID_ANY, "Horizon nodes", wxPoint(5, 105));
	text_hashHits = new wxStaticText(box_engineInfo, wxID_ANY, "Hashtable Hits", wxPoint(5, 120));
	checkbox_drawUpdates = new wxCheckBox(box_engineInfo, wxID_ANY, "Draw Updates", wxPoint(5, 135));
	checkbox_blackAutorespond->Set3StateValue(wxCheckBoxState::wxCHK_CHECKED);

	box_drawMode = new wxStaticBox(this, wxID_ANY, "Draw Mode", wxPoint(5, 670), wxSize(190, 60));
	radiobtn_pugsandbears = new wxRadioButton(box_drawMode, 10007, "Pugs and Bears", wxPoint(5,20));
	radiobtn_classic = new wxRadioButton(box_drawMode, 10008, "Classics", wxPoint(5, 35));
	radiobtn_pugsandbears->SetValue(true);

	engineTimer = new wxTimer(this, 10004);

	sizer->AddSpacer(200);

	drawPane = new BasicDrawPane((wxFrame*)this);
	sizer->Add(drawPane, 1, wxEXPAND);

	this->SetSizer(sizer);
	this->SetAutoLayout(true);
	updateControls();
}

cMain::~cMain() {
	if (state == appState::EngineComputing) {
		engine->timeLimit = 0;
		checkbox_blackAutorespond->Set3StateValue(wxCheckBoxState::wxCHK_UNCHECKED);
		checkbox_whiteAutorespond->Set3StateValue(wxCheckBoxState::wxCHK_UNCHECKED);
	}
}

void cMain::sizeEvent(wxSizeEvent& event) {
	drawPane->paintNow();
	event.Skip();
	drawPane->Refresh();
}

void cMain::resetStatsClicked(wxCommandEvent& event){
	gameNumber = 0;
	whiteWins = 0;
	blackWins = 0;
	draws = 0;
	updateControls();
}

void cMain::undoMoveClicked(wxCommandEvent& event) {
	if (state == appState::EngineComputing) return;
	if (board->getPlyCount() > 0) {
		board->undoMove(moveHistory.back());
		moveHistory.pop_back();
		if (state == appState::BlackCheckmate) {
			whiteWins--;
			gameNumber--;
		}
		else if (state == appState::WhiteCheckmate) {
			blackWins--;
			gameNumber--;
		}
		else if (state == appState::Draw) {
			draws--;
			gameNumber--;
		}
		else colour = switchColour(colour);
		if (draws < 0) draws = 0;
		if (whiteWins < 0) whiteWins = 0;
		if (blackWins < 0) blackWins = 0;
		if (gameNumber < 0) gameNumber = 0;
		writePieceInfo(board, squares);
		state = appState::Idle;
		updateGameState();
		updateControls();
		drawPane->paintNow();
	}
	event.Skip();
}


void cMain::aiMoveClicked(wxCommandEvent& event) {
	if (state == appState::EngineComputing) return;
	engineTimer->Start(100);
	std::thread t1(&cMain::calculateAIMove, this);
	ai_thread = &t1;
	t1.detach();
	event.Skip();
}

void cMain::aiAbortClicked(wxCommandEvent& event) {
	if (state != appState::EngineComputing) return;
	setGaugeToMax();
	engine->timeLimit = 0;
	event.Skip();
}

void cMain::resetBoardClicked(wxCommandEvent& event)
{
	if (state == appState::EngineComputing) return;
	board->resetBoard();
	colour = Colour::White;
	moveHistory.clear();
	writePieceInfo(board, squares);
	state = appState::Idle;
	updateControls();
	drawPane->paintNow();
	event.Skip();
}

void cMain::radioButtonClicked(wxCommandEvent& event)
{
	drawPane->paintNow();
}

void cMain::updateControls() {
	if (board->getPlyCount() == 0) text_lastMove->SetLabel("Last Move: n/a");
	else {
		char xOrig = std::get<0>(moveHistory.back().orig);
		char yOrig = std::get<1>(moveHistory.back().orig);
		char xDest = std::get<0>(moveHistory.back().dest);
		char yDest = std::get<1>(moveHistory.back().dest);
		std::string m = numToLetter(xOrig) + std::to_string(yOrig + 1) + " " + numToLetter(xDest) + std::to_string(yDest + 1);
		text_lastMove->SetLabel("Last Move: "+m);
	}
	if (state == appState::Draw) {
		text_legalMoveCount->SetLabel("Possible Moves: n/a");
		text_captureMoveCount->SetLabel("Possible Captures: n/a");
		text_turn->SetLabel("[" + std::to_string(board->getPlyCount() + 1) + "]" + " Draw.");
	}
	else if (state == appState::WhiteCheckmate) {
		text_legalMoveCount->SetLabel("Possible Moves: n/a");
		text_captureMoveCount->SetLabel("Possible Captures: n/a");
		text_turn->SetLabel("[" + std::to_string(board->getPlyCount() + 1) + "]" + " White is Checkmate!");
	}
	else if (state == appState::BlackCheckmate) {
		text_legalMoveCount->SetLabel("Possible Moves: n/a");
		text_captureMoveCount->SetLabel("Possible Captures: n/a");
		text_turn->SetLabel("[" + std::to_string(board->getPlyCount() + 1) + "]" + " Black is Checkmate!");
	}
	else if (state != appState::EngineComputing) {
		std::vector<std::tuple<char, char, char, char>> moveList, captureList;
		moveList = board->getPossibleMoves(colour);
		captureList = board->getPossibleCaptures(colour);
		text_legalMoveCount->SetLabel("Possible Moves: " + std::to_string(moveList.size()));
		text_captureMoveCount->SetLabel("Possible Captures: " + std::to_string(captureList.size()));
		if (colour == Colour::White) text_turn->SetLabel("[" + std::to_string(board->getPlyCount() + 1) + "]" + " White's turn.");
		else text_turn->SetLabel("[" + std::to_string(board->getPlyCount() + 1) + "]" + " Black's turn.");
	}
	text_totalGames->SetLabel("Games played:  " + std::to_string(gameNumber));
	text_whiteWins->SetLabel("White wins:       " + std::to_string(whiteWins));
	text_blackWins->SetLabel("Black wins:        " + std::to_string(blackWins));
	text_draws->SetLabel("Draws:                " + std::to_string(draws));
}

void cMain::printUpdate(Engine* engine) {
	writePieceInfo(board, squares_intermediate);
}

void cMain::updateGameState() {
	if (colour == Colour::Black) {
		if (board->getPossibleMoves(Colour::White).empty()) {
			if (board->isChecked(Colour::White)) {
				blackWins++;
				gameNumber++;
				state = appState::WhiteCheckmate;
			}
			else {
				draws++;
				gameNumber++;
				state = appState::Draw;
			}
		}
		else state = appState::Idle;
	}
	else {
		if (board->getPossibleMoves(Colour::Black).empty()) {
			if (board->isChecked(Colour::Black)) {
				whiteWins++;
				gameNumber++;
				state = appState::BlackCheckmate;
			}
			else {
				draws++;
				gameNumber++;
				state = appState::Draw;
			}
		}
		else state = appState::Idle;
	}
}

void cMain::setGaugeToMax() {
	gauge_engineProgress->SetValue(gauge_engineProgress->GetRange());
	gauge_engineProgress->SetValue(gauge_engineProgress->GetRange() - 1);
	gauge_engineProgress->SetValue(gauge_engineProgress->GetRange());
}

void cMain::writePieceInfo(ChessBoard* orig, PieceInfo dest[8][8]) {
	for (int x = 0; x <= 7; x++)
		for (int y = 0; y <= 7; y++) {
			if (orig->squares[x][y] != nullptr) {
				dest[x][y].type = orig->squares[x][y]->getPieceType();
				dest[x][y].colour = orig->squares[x][y]->colour;
			}
			else
				dest[x][y].type = PieceType::None;
		}
}

void cMain::OnTimer(wxTimerEvent& event) {
	double timePassed = engine->getTimePassed();
	gauge_engineProgress->SetValue(timePassed * 1000);
	gauge_engineProgress->SetValue(timePassed * 1000-1);
	gauge_engineProgress->SetValue(timePassed * 1000);
	int xOrig = std::get<0>(engine->optimalTurnSequence.at(0));
	int yOrig = std::get<1>(engine->optimalTurnSequence.at(0));
	int xDest = std::get<2>(engine->optimalTurnSequence.at(0));
	int yDest = std::get<3>(engine->optimalTurnSequence.at(0));
	std::string m = numToLetter(xOrig) + std::to_string(yOrig + 1) + " " + numToLetter(xDest) + std::to_string(yDest + 1);
	text_moveData->SetLabel("opt=[" + m + "]@val " + std::to_string(engine->optimalValue));
	text_depth->SetLabel("Depth: " + std::to_string(engine->depthLimit) + "+" + std::to_string(engine->quiescenceLimit));
	text_timePassed->SetLabel("Time, speed: " + std::to_string(int(timePassed * 1000)) + " ms, " + std::to_string(int((engine->getNodes() + engine->getQuiescenceNodes()) / 1000 / timePassed)) + " kN/s");
	text_nodes->SetLabel("Normal nodes: " + std::to_string(engine->getNodes()));
	text_horizonNodes->SetLabel("Horizon nodes: " + std::to_string(engine->getQuiescenceNodes()));
	text_hashHits->SetLabel("Hashtable Hits: " + std::to_string(engine->getHashHits(board)));

	drawPane->paintNow();
	if (state == appState::EngineComputing) engineTimer->Start(10);
	else if ((state == appState::Idle) && (((colour == Colour::White) && checkbox_whiteAutorespond->IsChecked()) || ((colour == Colour::Black) && checkbox_blackAutorespond->IsChecked()))) {
		std::thread t1(&cMain::calculateAIMove, this); 
		t1.detach();
	}
	else {
		setGaugeToMax();
		engineTimer->Stop();
	}
}

void cMain::calculateAIMove() {
	if ((state == appState::Draw) || (state == appState::WhiteCheckmate) || (state == appState::BlackCheckmate)) return;
	state = appState::EngineComputing;
	double timeLimit;
	long quiescenceLimit;
	if (colour == Colour::White) {
		textctrl_whiteAllocTime->GetValue().ToDouble(&timeLimit);
		textctrl_whiteQuiescenceDepth->GetValue().ToLong(&quiescenceLimit);
		if (!checkbox_whiteRandomize->IsChecked()) {
			engine->timeLimit = timeLimit / 1000.0f;
			engine->quiescenceLimit = int(quiescenceLimit);
			gauge_engineProgress->SetRange(engine->timeLimit * 1000);
			gauge_engineProgress->SetValue(0);
			engine->updateFct = std::bind(&cMain::printUpdate, this, std::placeholders::_1);
			//Engine config
			engine->useHashtable = true;
			board->improvedDrawDetection = true;
			engine->useKingEndgameScoreboard = true;
			engine->nullmove = false;

			engine->calculateMove_iterativeDeepening(this->board, this->colour);
		}
		else engine->calculateMove_random(this->board, this->colour);
	}
	else {
		textctrl_blackAllocTime->GetValue().ToDouble(&timeLimit);
		textctrl_blackQuiescenceDepth->GetValue().ToLong(&quiescenceLimit);
		if (!checkbox_blackRandomize->IsChecked()) {
			engine->timeLimit = timeLimit / 1000.0f;
			engine->quiescenceLimit = int(quiescenceLimit);
			gauge_engineProgress->SetRange(engine->timeLimit * 1000);
			gauge_engineProgress->SetValue(0);
			engine->updateFct = std::bind(&cMain::printUpdate, this, std::placeholders::_1);
			//Engine config
			engine->useHashtable = true;
			board->improvedDrawDetection = true;
			engine->useKingEndgameScoreboard = true;
			engine->nullmove = false;

			engine->calculateMove_iterativeDeepening(this->board, this->colour);
		}
		else engine->calculateMove_random(this->board, this->colour);
	}
	setGaugeToMax();
	int xOrig = std::get<0>(engine->optimalTurnSequence.at(0));
	int yOrig = std::get<1>(engine->optimalTurnSequence.at(0));
	int xDest = std::get<2>(engine->optimalTurnSequence.at(0));
	int yDest = std::get<3>(engine->optimalTurnSequence.at(0));
	MoveData move = board->moveTo(std::tuple<char, char>(xOrig, yOrig), std::tuple<char, char>(xDest, yDest));
	moveHistory.push_back(move);
	updateGameState();
	if ((state != appState::Draw) && (state != appState::WhiteCheckmate) && (state != appState::BlackCheckmate)) {
		if (move.validMove) colour = switchColour(colour);
		state = appState::Idle;
	}
	writePieceInfo(board, squares);
	updateControls();
	drawPane->paintNow();
}



BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)

	EVT_PAINT(BasicDrawPane::paintEvent)
	EVT_LEFT_DOWN(BasicDrawPane::mouseDown)
	EVT_RIGHT_DOWN(BasicDrawPane::mouseDown)

END_EVENT_TABLE()

BasicDrawPane::BasicDrawPane(wxFrame* parent) : wxPanel(parent) {
	wxInitAllImageHandlers();
	SetDoubleBuffered(true);
	m_background_orig = *GetResourceInputStream(IDB_BOARD, L"PNG");
	m_Bb_orig = *GetResourceInputStream(IDB_BB, L"PNG");
	m_Bw_orig = *GetResourceInputStream(IDB_BW, L"PNG");
	m_Kb_orig = *GetResourceInputStream(IDB_KB, L"PNG");
	m_Kw_orig = *GetResourceInputStream(IDB_KW, L"PNG");
	m_Pb_orig = *GetResourceInputStream(IDB_PB, L"PNG");
	m_Pw_orig = *GetResourceInputStream(IDB_PW, L"PNG");
	m_Qb_orig = *GetResourceInputStream(IDB_QB, L"PNG");
	m_Qw_orig = *GetResourceInputStream(IDB_QW, L"PNG");
	m_Rb_orig = *GetResourceInputStream(IDB_RB, L"PNG");
	m_Rw_orig = *GetResourceInputStream(IDB_RW, L"PNG");
	m_Xb_orig = *GetResourceInputStream(IDB_XB, L"PNG");
	m_Xw_orig = *GetResourceInputStream(IDB_XW, L"PNG");

	m_Bb_classic_orig = *GetResourceInputStream(IDB_BB_CLASSIC, L"PNG");
	m_Bw_classic_orig = *GetResourceInputStream(IDB_BW_CLASSIC, L"PNG");
	m_Kb_classic_orig = *GetResourceInputStream(IDB_KB_CLASSIC, L"PNG");
	m_Kw_classic_orig = *GetResourceInputStream(IDB_KW_CLASSIC, L"PNG");
	m_Pb_classic_orig = *GetResourceInputStream(IDB_PB_CLASSIC, L"PNG");
	m_Pw_classic_orig = *GetResourceInputStream(IDB_PW_CLASSIC, L"PNG");
	m_Qb_classic_orig = *GetResourceInputStream(IDB_QB_CLASSIC, L"PNG");
	m_Qw_classic_orig = *GetResourceInputStream(IDB_QW_CLASSIC, L"PNG");
	m_Rb_classic_orig = *GetResourceInputStream(IDB_RB_CLASSIC, L"PNG");
	m_Rw_classic_orig = *GetResourceInputStream(IDB_RW_CLASSIC, L"PNG");
	m_Xb_classic_orig = *GetResourceInputStream(IDB_XB_CLASSIC, L"PNG");
	m_Xw_classic_orig = *GetResourceInputStream(IDB_XW_CLASSIC, L"PNG");
}


void BasicDrawPane::paintEvent(wxPaintEvent& evt)
{
	wxPaintDC dc(this);
	render(dc);
}

void BasicDrawPane::paintNow()
{
	wxClientDC dc(this);
	render(dc);
}

void BasicDrawPane::render(wxDC& dc)
{
	cMain* parent = (cMain*)this->GetParent();
	wxSize size = this->GetSize();
	int paneWidthHeight = std::min(size.GetWidth(), size.GetHeight());
	paneWidthHeight = std::max(paneWidthHeight, 1);
	int pieceWidthHeight = std::max(paneWidthHeight / 8,1);
	wxImage m_Bb, m_Bw, m_Kb, m_Kw, m_Pb, m_Pw, m_Qb, m_Qw, m_Rb, m_Rw, m_Xb, m_Xw;
	if (parent->radiobtn_pugsandbears->GetValue()) {
		m_Bb = m_Bb_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Bw = m_Bw_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Kb = m_Kb_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Kw = m_Kw_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Pb = m_Pb_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Pw = m_Pw_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Qb = m_Qb_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Qw = m_Qw_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Rb = m_Rb_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Rw = m_Rw_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Xb = m_Xb_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Xw = m_Xw_orig.Scale(pieceWidthHeight, pieceWidthHeight);
	}
	else {
		m_Bb = m_Bb_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Bw = m_Bw_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Kb = m_Kb_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Kw = m_Kw_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Pb = m_Pb_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Pw = m_Pw_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Qb = m_Qb_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Qw = m_Qw_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Rb = m_Rb_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Rw = m_Rw_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Xb = m_Xb_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
		m_Xw = m_Xw_classic_orig.Scale(pieceWidthHeight, pieceWidthHeight);
	}
	wxImage m_background = m_background_orig.Scale(paneWidthHeight, paneWidthHeight);
	dc.DrawBitmap(m_background, 0, 0, false);
	for (int x = 0; x <= 7; x++)
		for (int y = 0; y <= 7; y++) {
			if ((parent->state == appState::PieceSelected) && (x == std::get<0>(parent->selectedPiece)) && (y == std::get<1>(parent->selectedPiece))) {
				dc.SetPen(wxPen(wxColor(0, 0, 0), 1));
				dc.DrawCircle(wxPoint((x + 0.5f) * pieceWidthHeight, ((7 - y + 0.5f)) * pieceWidthHeight), pieceWidthHeight / 2); //radius
			}
			if (parent->squares[x][y].type != PieceType::None) {
				if (parent->squares[x][y].colour == Colour::White) {
					if (parent->squares[x][y].type == PieceType::Pawn)
						dc.DrawBitmap(m_Pw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::Rook)
						dc.DrawBitmap(m_Rw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::Bishop)
						dc.DrawBitmap(m_Bw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::Knight)
						dc.DrawBitmap(m_Kw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::King)
						dc.DrawBitmap(m_Xw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::Queen)
						dc.DrawBitmap(m_Qw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
				}
				else {
					if (parent->squares[x][y].type == PieceType::Pawn)
						dc.DrawBitmap(m_Pb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::Rook)
						dc.DrawBitmap(m_Rb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::Bishop)
						dc.DrawBitmap(m_Bb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::Knight)
						dc.DrawBitmap(m_Kb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::King)
						dc.DrawBitmap(m_Xb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					if (parent->squares[x][y].type == PieceType::Queen)
						dc.DrawBitmap(m_Qb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
				}
			}
		}

	if ((parent->state == appState::EngineComputing) && (parent->checkbox_drawUpdates->IsChecked())) {
		unsigned char* alphaData_Bb = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Bw = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Kw = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Kb = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Pb = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Pw = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Qb = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Qw = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Rb = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Rw = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Xb = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		unsigned char* alphaData_Xw = new unsigned char[pieceWidthHeight * pieceWidthHeight];
		memset(alphaData_Bb, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Bw, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Kw, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Kb, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Pb, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Pw, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Qw, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Rb, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Rw, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Xb, 127, pieceWidthHeight * pieceWidthHeight);
		memset(alphaData_Xw, 127, pieceWidthHeight * pieceWidthHeight);
		m_Bb.SetAlpha(alphaData_Bb);
		m_Bw.SetAlpha(alphaData_Bw);
		m_Kb.SetAlpha(alphaData_Kb);
		m_Kw.SetAlpha(alphaData_Kw);
		m_Pb.SetAlpha(alphaData_Pb);
		m_Pw.SetAlpha(alphaData_Pw);
		m_Qb.SetAlpha(alphaData_Qb);
		m_Qw.SetAlpha(alphaData_Qw);
		m_Rb.SetAlpha(alphaData_Rb);
		m_Rw.SetAlpha(alphaData_Rw);
		m_Xb.SetAlpha(alphaData_Xb);
		m_Xw.SetAlpha(alphaData_Xw);
		
		for (int x = 0; x <= 7; x++)
			for (int y = 0; y <= 7; y++) {
				if (parent->squares_intermediate[x][y].type != PieceType::None) {
					if (parent->squares_intermediate[x][y].colour == Colour::White) {
						if (parent->squares_intermediate[x][y].type == PieceType::Pawn)
							dc.DrawBitmap(m_Pw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::Rook)
							dc.DrawBitmap(m_Rw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::Bishop)
							dc.DrawBitmap(m_Bw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::Knight)
							dc.DrawBitmap(m_Kw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::King)
							dc.DrawBitmap(m_Xw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::Queen)
							dc.DrawBitmap(m_Qw, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					}
					else {
						if (parent->squares_intermediate[x][y].type == PieceType::Pawn)
							dc.DrawBitmap(m_Pb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::Rook)
							dc.DrawBitmap(m_Rb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::Bishop)
							dc.DrawBitmap(m_Bb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::Knight)
							dc.DrawBitmap(m_Kb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::King)
							dc.DrawBitmap(m_Xb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
						if (parent->squares_intermediate[x][y].type == PieceType::Queen)
							dc.DrawBitmap(m_Qb, x * pieceWidthHeight, (7 - y) * pieceWidthHeight, true);
					}
				}
			}
	}

	std::vector<std::tuple<char, char>> moveList;
	if (parent->state == appState::PieceSelected) {
		char x = std::get<0>(parent->selectedPiece);
		char y = std::get<1>(parent->selectedPiece);
		moveList = parent->board->squares[x][y]->getMoveList(parent->board->squares, x, y);

		for (unsigned int i = 0; i < moveList.size(); i++) {
			char x = std::get<0>((moveList)[i]);
			char y = std::get<1>((moveList)[i]);
			dc.SetPen(wxPen(wxColor(0, 0, 0), 1));
			dc.DrawLine((x + 0.25f) * pieceWidthHeight, ((7 - y + 0.25f)) * pieceWidthHeight, (x + 0.75f) * pieceWidthHeight, ((7 - y + 0.75f)) * pieceWidthHeight);
			dc.DrawLine((x + 0.25f) * pieceWidthHeight, ((7 - y + 0.75f)) * pieceWidthHeight, (x + 0.75f) * pieceWidthHeight, ((7 - y + 0.25f)) * pieceWidthHeight);
		}

	}

	if (!parent->moveHistory.empty() && (parent->board->getPlyCount() > 0) && (parent->drawLastMove)) {
		std::tuple<int, int> vect;
		int xOrig, yOrig;
		MoveData lastMove = parent->moveHistory.back();
		xOrig = std::get<0>(lastMove.orig);
		yOrig = std::get<1>(lastMove.orig);
		vect = std::make_pair((std::get<0>(lastMove.dest) - std::get<0>(lastMove.orig)) * pieceWidthHeight, (std::get<1>(lastMove.dest) - std::get<1>(lastMove.orig)) * pieceWidthHeight);

		int arrowLeftX = -pieceWidthHeight / 4;
		int arrowLeftY = pieceWidthHeight / 4;
		int arrowRightX = -pieceWidthHeight / 4;
		int arrowRightY = -pieceWidthHeight / 4;

		double theta = std::atan2(std::get<1>(vect), std::get<0>(vect));

		int arrowLeftXrot = arrowLeftX * std::cos(theta) - arrowLeftY * std::sin(theta);
		int arrowLeftYrot = -(arrowLeftX * std::sin(theta) + arrowLeftY * std::cos(theta));
		int arrowRightXrot = arrowRightX * std::cos(theta) - arrowRightY * std::sin(theta);
		int arrowRightYrot = -(arrowRightX * std::sin(theta) + arrowRightY * std::cos(theta));

		int dist = std::sqrt((std::get<0>(vect) * std::get<0>(vect) + std::get<1>(vect) * std::get<1>(vect)));
		double scalar = 1 - std::sqrt(2 * pieceWidthHeight * pieceWidthHeight)/(3*dist);
		int arrowTargetX = (xOrig + 0.5f) * pieceWidthHeight + std::get<0>(vect) * scalar;
		int arrowTargetY = (7 - yOrig + 0.5f) * pieceWidthHeight - std::get<1>(vect) * scalar;
		dc.SetPen(wxPen(wxColor(0, 0, 0), 1));
		dc.DrawLine((xOrig + 0.5f) * pieceWidthHeight, ((7 - yOrig + 0.5f)) * pieceWidthHeight, 
			arrowTargetX, arrowTargetY);
		dc.DrawLine(wxPoint(arrowTargetX, arrowTargetY), wxPoint(arrowTargetX + arrowLeftXrot, arrowTargetY + arrowLeftYrot));
		dc.DrawLine(wxPoint(arrowTargetX, arrowTargetY), wxPoint(arrowTargetX + arrowRightXrot, arrowTargetY + arrowRightYrot));
																																														 
	}

	if (parent->drawLastMove) {
		wxFont font;
		font.SetPixelSize(wxSize(pieceWidthHeight * 0.2 + 1, pieceWidthHeight * 0.4 + 1));
		dc.SetFont(font);
		dc.SetPen(wxPen(wxColor(255, 255, 255), 5));
		if (parent->state == appState::BlackCheckmate) dc.DrawText(wxT("Black is checkmate"), paneWidthHeight / 2 - pieceWidthHeight * 2, paneWidthHeight / 2 - pieceWidthHeight / 4);
		else if (parent->state == appState::WhiteCheckmate) dc.DrawText(wxT("White is checkmate"), paneWidthHeight / 2 - pieceWidthHeight * 2, paneWidthHeight / 2 - pieceWidthHeight / 4);
		else if (parent->state == appState::Draw) dc.DrawText(wxT("Stalemate"), paneWidthHeight / 2 - pieceWidthHeight, paneWidthHeight / 2 - pieceWidthHeight / 4);
	}
}


void BasicDrawPane::mouseDown(wxMouseEvent& event) {
	wxSize size = this->GetSize();
	cMain* parent = (cMain*)this->GetParent();
	if (event.LeftDown()) {
		if ((parent->state == appState::Draw) || (parent->state == appState::WhiteCheckmate) || (parent->state == appState::BlackCheckmate)) return;
		wxSize size = this->GetSize();
		cMain* parent = (cMain*)this->GetParent();
		int paneWidthHeight = std::min(size.GetWidth(), size.GetHeight());
		int pieceWidthHeight = paneWidthHeight / 8;
		char x = event.GetX() / pieceWidthHeight;
		char y = 7 - event.GetY() / pieceWidthHeight;

		if (parent->state == appState::PieceSelected) {
			MoveData move = parent->board->moveTo(parent->selectedPiece, std::tuple<char, char>(x, y));
		
			if (move.validMove) {
				parent->updateGameState();
				if ((parent->state != appState::Draw) && (parent->state != appState::WhiteCheckmate) && (parent->state != appState::BlackCheckmate))
					parent->colour = switchColour(parent->colour);
				parent->writePieceInfo(parent->board, parent->squares);
				parent->moveHistory.push_back(move);
				parent->updateControls();
				paintNow();
				if (((parent->colour == Colour::White) && parent->checkbox_whiteAutorespond->IsChecked()) || ((parent->colour == Colour::Black) && parent->checkbox_blackAutorespond->IsChecked())) {
					parent->engineTimer->Start(100);
					std::thread t1(&cMain::calculateAIMove, parent);
					parent->ai_thread = &t1;
					t1.detach();
				}
				return;
			}
		}
		if ((parent->board->squares[x][y] != nullptr) && (parent->board->squares[x][y]->colour == parent->colour)) {
			parent->selectedPiece = std::tuple<char, char>(x, y);
			parent->state = appState::PieceSelected;
			parent->updateControls();
		}

		paintNow();
	}
	else if (event.RightDown()) {
		parent->drawLastMove = !parent->drawLastMove;
		paintNow();
	}
}

