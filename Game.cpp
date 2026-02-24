#pragma once

#include "Game.h"
#include "ChessBoard.h"
#include <iostream>
#include <exception>
// #include "Pawn.h"

Game::Game(){
    int H = ChessBoard::Chessboard->HEIGHT;
    int W = ChessBoard::Chessboard->WIDTH;
	window.create(sf::VideoMode(H, W), "ChessBoard");

    // rotate the window by 90
    view.setSize(window.getSize().x, window.getSize().y);
    view.setCenter(window.getSize().x / 2.f, window.getSize().y / 2.f);
    view.setRotation(90);

    window.setView(view);

    updateState = true;
}

void Game::Initialize()
{
    int H = ChessBoard::Chessboard->HEIGHT;
    int W = ChessBoard::Chessboard->WIDTH;

    sf::Sprite& whiteTile = ChessBoard::Chessboard->WhiteTile;
    sf::Sprite& blackTile = ChessBoard::Chessboard->BlackTile;

    bool flag = true;
    for (int x = 0; x < W; x += W / 8) {
        for (int y = 0; y < H; y += H / 8) {
            flag ? whiteTile.setPosition(x, y) : blackTile.setPosition(x, y);
            sf::Sprite tile = flag ? whiteTile : blackTile;

            tileLayout.push_back(tile);
            flag = !flag;
        }
        flag = !flag;
    }
}

void Game::Update() {

    sf::Vector2i dragOrigin;
    Piece* selectedPiece = nullptr;
    bool isDragging = false;

    Game::Initialize();
    Game::InitializeBoard();

    while (window.isOpen()) {

        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
        }
        window.clear();

        for (auto& tiles : tileLayout) {
            window.draw(tiles);
        }

        Game::PieceDragLogic(e, isDragging, selectedPiece, dragOrigin);

        if (isDragging && selectedPiece) {
            if (selectedPiece->color == ChessBoard::Chessboard->player) {
                const std::vector<sf::Vector2i>& legalMoves = selectedPiece->legalMoves(dragOrigin, GameState::PLAYER);

                updatePieceCordinates(e, isDragging, selectedPiece, dragOrigin, legalMoves);

                if (ChessBoard::InBounds(dragOrigin) && ChessBoard::Chessboard->board[dragOrigin.x][dragOrigin.y])
                    Game::highLightMoves(selectedPiece, dragOrigin, legalMoves);
            }
        }

        Game::UpdatePieces();

        window.display();
    }
}

void Game::InitializeBoard()
{
    vector<vector<Piece*>> chessBoard = ChessBoard::Chessboard->board;
    int H = ChessBoard::Chessboard->HEIGHT / 8;
    int W = ChessBoard::Chessboard->WIDTH / 8;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (chessBoard[x][y]) {
                sf::Sprite& cur = chessBoard[x][y]->GetSprite();
                cur.setPosition((x+1) * H, y * W);
                cur.setRotation(90);
                window.draw(cur);
            }
        }
    }
}

void Game::UpdatePieces()
{
    vector<vector<Piece*>> chessBoard = ChessBoard::Chessboard->board;
    int H = ChessBoard::Chessboard->HEIGHT / 8;
    int W = ChessBoard::Chessboard->WIDTH / 8;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (chessBoard[x][y]) {
                sf::Sprite& cur = chessBoard[x][y]->GetSprite();
                window.draw(cur);
            }
        }
    }
}

void Game::PieceDragLogic(sf::Event& event, bool& isDragging, Piece*& selectedPiece, sf::Vector2i& dragOrigin)
{
    // Mouse drag: select the piece
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        int H = ChessBoard::Chessboard->HEIGHT / 8;
        int W = ChessBoard::Chessboard->WIDTH / 8;

        dragOrigin = sf::Vector2i((pos.x + 1) / H, pos.y / W);
        selectedPiece = ChessBoard::Chessboard->board[dragOrigin.x][dragOrigin.y];

        if (selectedPiece) {
            isDragging = true;
        }
    }
}

void Game::updatePieceCordinates(sf::Event& event, bool& isDragging, Piece*& selectedPiece, sf::Vector2i& dragOrigin, const std::vector<sf::Vector2i>& legalMoves)
{
    // update game state if piece is being dragged do this only for the first dragging frame
    if (!updateState) {
        MoveGenerator::KingMoves(ChessBoard::Chessboard->BlackKing.second, GameState::GAME);
        MoveGenerator::KingMoves(ChessBoard::Chessboard->WhiteKing.second, GameState::GAME);
        updateState = true;

		if (ChessBoard::Chessboard->WhiteKing.first->checkmate) {
			cout << "WHITE KING IS CHECKMATED BLACK KING WINS!" << endl;
        }
        else if (ChessBoard::Chessboard->BlackKing.first->checkmate) {
			cout << "BLACK KING IS CHECKMATED WHITE KING WINS!" << endl;
        }
    }

    sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

    sf::FloatRect bounds = selectedPiece->GetSprite().getLocalBounds();
    selectedPiece->GetSprite().setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    selectedPiece->GetSprite().setPosition(pos);

    // Mouse release: drop the piece
    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {

        if (isDragging && selectedPiece) {
            sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            int H = ChessBoard::Chessboard->HEIGHT / 8;
            int W = ChessBoard::Chessboard->WIDTH / 8;

            sf::Vector2i dropIndex((pos.x + 1) / H, pos.y / W);

            // Snap to grid
            selectedPiece->GetSprite().setPosition((dropIndex.x + 1) * H, dropIndex.y * W);
            selectedPiece->GetSprite().setOrigin(0, 0);

            // Update board
            if (dropIndex != dragOrigin) {
                if (find(legalMoves.begin(), legalMoves.end(), dropIndex) != legalMoves.end()) {

                    cout << "LEGAL MOVE BEING MADE!!!" << endl;
                    ChessBoard::Chessboard->board[dropIndex.x][dropIndex.y] = selectedPiece;
                    ChessBoard::Chessboard->board[dragOrigin.x][dragOrigin.y] = nullptr;

					// Special enpassant logic for pawns
                    if (selectedPiece->pieceType == ChessPiece::PAWN) {
                        auto pawn = dynamic_cast<Pawn*>(selectedPiece);

						// Handle en-passant capture
                        if (pawn->getEnPassantTarget() != sf::Vector2i()) {
							sf::Vector2i capturedPawnIndex = pawn->getEnPassantTarget();
							int direction = (selectedPiece->getColor() == Turn::WHITE) ? 1 : -1;
                            if (dropIndex == sf::Vector2i({ capturedPawnIndex.x + direction, capturedPawnIndex.y })) {
								cout << "EN-PASSANT CAPTURE MADE AT: " << capturedPawnIndex.x << ", " << capturedPawnIndex.y << endl;
								delete ChessBoard::Chessboard->board[capturedPawnIndex.x][capturedPawnIndex.y];
								ChessBoard::Chessboard->board[capturedPawnIndex.x][capturedPawnIndex.y] = nullptr;
                            }
                        }
						// Set en-passant target for opponent pawns if pawn moved two squares
                        try {
                            if (abs(dropIndex.x - dragOrigin.x) == 2) {
                                vector<sf::Vector2i> potencialenpassant = { {dropIndex.x, dropIndex.y - 1}, {dropIndex.x, dropIndex.y + 1} };
                                for (auto target : potencialenpassant) {
                                    if (ChessBoard::Chessboard->InBounds(target) && ChessBoard::Chessboard->board[target.x][target.y] && ChessBoard::Chessboard->board[target.x][target.y]->pieceType == ChessPiece::PAWN) {
                                        auto enpassantPawn = dynamic_cast<Pawn*>(ChessBoard::Chessboard->board[target.x][target.y]);
                                        enpassantPawn->setEnPassantTarget(sf::Vector2i(dropIndex.x, dropIndex.y));
                                        cout << "EN-PASSANT TARGET SET FOR PAWN AT: " << target.x << ", " << target.y << " TO " << dropIndex.x << ", " << dropIndex.y << endl;
                                    }
                                }
                            }
                            pawn->setMoved(true);
                            pawn->setEnPassantTarget(sf::Vector2i());
                        }
                        catch (exception e) {
                            cout << "Exception errro in en-passant logic " << e.what() << endl;
                        }
                    }
                    else if (selectedPiece->pieceType == ChessPiece::ROOK) {
                        Rook* rook = dynamic_cast<Rook*>(selectedPiece);
                        rook->hasMoved = true;
                    }

                    // Update White/Black king for there chessBoard references 
                    if (selectedPiece->pieceType == ChessPiece::KING) {
                        selectedPiece->color == Turn::WHITE ? ChessBoard::Chessboard->WhiteKing.second = dropIndex : ChessBoard::Chessboard->BlackKing.second = dropIndex;
                        selectedPiece->color == Turn::WHITE ? ChessBoard::Chessboard->WhiteKing.first->hasMoved = true : ChessBoard::Chessboard->BlackKing.first->hasMoved = true;
                    }

                    selectedPiece->color == Turn::WHITE ? ChessBoard::Chessboard->WhiteKing.first->inCheck = false : ChessBoard::Chessboard->BlackKing.first->inCheck = false;
                    bool isSelectedPiecePinned = selectedPiece->pinnedPiece != sf::Vector2i({ 0, 0 });
                    if (isSelectedPiecePinned) {
                        sf::Vector2i idx = selectedPiece->pinnedPiece;
                        ChessBoard::Chessboard->board[idx.x][idx.y]->PinningPiece = pair<sf::Vector2i, sf::Vector2i>();
                        selectedPiece->pinnedPiece = sf::Vector2i();
                    }

                    ChessBoard::Chessboard->player = ChessBoard::Chessboard->player == Turn::BLACK ? Turn::WHITE : Turn::BLACK;

                    selectedPiece = nullptr;
                    isDragging = false;
                    updateState = false;

					cout << "Board State updated to: " << ChessBoard::Chessboard->getBoardState() << endl;

					ChessBoard::Chessboard->WhiteKing.first->checkmate = ChessBoard::Chessboard->WhiteKing.first->inCheck;
					ChessBoard::Chessboard->BlackKing.first->checkmate = ChessBoard::Chessboard->BlackKing.first->inCheck;

                }
                else {
                    isDragging = false;
                    selectedPiece->GetSprite().setOrigin(0, 0);
                    selectedPiece->GetSprite().setPosition((dragOrigin.x + 1) * H, dragOrigin.y * W);
                }
            }
            else {
                isDragging = false;
                selectedPiece->GetSprite().setOrigin(0, 0);
            }
        }
    }
}

void Game::highLightMoves(Piece* selectedPiece, sf::Vector2i& dragOrigin, const vector<sf::Vector2i>& legalMoves)
{
    if (!selectedPiece) return;
    cout << dragOrigin.x << ',' << dragOrigin.y << endl;

    int tileWidth = ChessBoard::Chessboard->WIDTH / 8;
    int tileHeight = ChessBoard::Chessboard->HEIGHT / 8;

    if (legalMoves.empty())
        return;

    std::cout << "FOUND MOVES!!!" << std::endl;
    sf::Sprite highlightTile = ChessBoard::Chessboard->highlightTile;

    for (const auto& move : legalMoves) {
        highlightTile.setPosition(move.x * tileWidth, move.y * tileHeight);
        window.draw(highlightTile);
    }
}



