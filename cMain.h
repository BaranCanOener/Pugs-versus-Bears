#pragma once
#include "wx/wx.h"
#include "wx/sizer.h"
#include "../CppChessEngine/board.h"
#include "../CppChessEngine/pieces.h"
#include "../CppChessEngine/engine.h"
#include "wx/spinbutt.h"
#include <thread>

class BasicDrawPane : public wxPanel
{

public:
	BasicDrawPane(wxFrame* parent);

	unsigned char* alphaData;

	wxImage m_background_orig;
	wxImage m_Bb_orig;
	wxImage m_Bw_orig;
	wxImage m_Kb_orig;
	wxImage m_Kw_orig;
	wxImage m_Pb_orig;
	wxImage m_Pw_orig;
	wxImage m_Qb_orig;
	wxImage m_Qw_orig;
	wxImage m_Rb_orig;
	wxImage m_Rw_orig;
	wxImage m_Xb_orig;
	wxImage m_Xw_orig;
	wxImage m_Xw_classic_orig;
	wxImage m_Bb_classic_orig;
	wxImage m_Bw_classic_orig;
	wxImage m_Kb_classic_orig;
	wxImage m_Kw_classic_orig;
	wxImage m_Pb_classic_orig;
	wxImage m_Pw_classic_orig;
	wxImage m_Qb_classic_orig;
	wxImage m_Qw_classic_orig;
	wxImage m_Rb_classic_orig;
	wxImage m_Rw_classic_orig;
	wxImage m_Xb_classic_orig;

	void paintEvent(wxPaintEvent& evt);
	void paintNow();

	void render(wxDC& dc);
	void mouseDown(wxMouseEvent& event);

	DECLARE_EVENT_TABLE()
};

enum class appState {Idle, EngineComputing, PieceSelected, Draw, WhiteCheckmate, BlackCheckmate};

struct PieceInfo {
	PieceType type;
	Colour colour;
};

class cMain : public wxFrame
{
public:
	cMain();
	~cMain();

	ChessBoard* board;
	Engine* engine;
	Colour colour = Colour::White;
	appState state = appState::Idle;
	std::thread* ai_thread;
	std::vector<MoveData> moveHistory;
	int gameNumber = 0;
	int whiteWins = 0;
	int blackWins = 0;
	int draws = 0;
	std::tuple<char, char> selectedPiece = std::tuple<char, char>(127,127);
	bool drawLastMove = true;
	PieceInfo squares[8][8];
	PieceInfo squares_intermediate[8][8];

	BasicDrawPane* drawPane = nullptr;
	
	wxStaticBox* box_gameStats = nullptr;
	wxStaticText* text_totalGames = nullptr;
	wxStaticText* text_whiteWins = nullptr;
	wxStaticText* text_blackWins = nullptr;
	wxStaticText* text_draws = nullptr;
	wxButton* btn_resetStats = nullptr;

	wxStaticBox* box_gameControls = nullptr;
	wxStaticText* text_turn = nullptr;
	wxStaticText* text_lastMove = nullptr;
	wxStaticText* text_legalMoveCount = nullptr;
	wxStaticText* text_captureMoveCount = nullptr;
	wxButton* btn_undoMove = nullptr;
	wxButton* btn_resetBoard = nullptr;
	wxButton* btn_aiMove = nullptr;
	wxButton* btn_abortAiMove = nullptr;

	wxStaticBox* box_whiteEngineControls = nullptr;
	wxStaticText* text_whiteAllocTime = nullptr;
	wxTextCtrl* textctrl_whiteAllocTime = nullptr;
	wxStaticText* text_whiteQuiescenceDepth = nullptr;
	wxTextCtrl* textctrl_whiteQuiescenceDepth = nullptr;
	wxCheckBox* checkbox_whiteRandomize = nullptr;
	wxCheckBox* checkbox_whiteAutorespond = nullptr;
	
	wxStaticBox* box_blackEngineControls = nullptr;
	wxStaticText* text_blackAllocTime = nullptr;
	wxTextCtrl* textctrl_blackAllocTime = nullptr;
	wxStaticText* text_blackQuiescenceDepth = nullptr;
	wxTextCtrl* textctrl_blackQuiescenceDepth = nullptr;
	wxCheckBox* checkbox_blackRandomize = nullptr;
	wxCheckBox* checkbox_blackAutorespond = nullptr;

	wxStaticBox* box_engineInfo = nullptr;
	wxGauge* gauge_engineProgress = nullptr;
	wxStaticText* text_moveData = nullptr;
	wxStaticText* text_depth = nullptr;
	wxStaticText* text_timePassed = nullptr;
	wxStaticText* text_nodes = nullptr;
	wxStaticText* text_horizonNodes = nullptr;
	wxStaticText* text_hashHits = nullptr;
	wxCheckBox* checkbox_drawUpdates = nullptr;

	wxStaticBox* box_drawMode = nullptr;
	wxRadioButton* radiobtn_pugsandbears = nullptr;
	wxRadioButton* radiobtn_classic = nullptr;

	wxTimer* engineTimer = nullptr;

	void sizeEvent(wxSizeEvent& event);
	void resetStatsClicked(wxCommandEvent& event);
	void undoMoveClicked(wxCommandEvent& event);
	void aiMoveClicked(wxCommandEvent& event);
	void aiAbortClicked(wxCommandEvent& event);
	void resetBoardClicked(wxCommandEvent& event);
	void radioButtonClicked(wxCommandEvent& event);
	void OnTimer(wxTimerEvent& event);
	void updateControls();
	void calculateAIMove();
	void printUpdate(Engine* engine);
	void updateGameState();
	void setGaugeToMax();
	void writePieceInfo(ChessBoard* orig, PieceInfo dest[8][8]);

	DECLARE_EVENT_TABLE()
};