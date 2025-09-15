#pragma once

#include "Game.h"
#include "ChessBoard.h"
#include <iostream>
#include <exception>
#include "Pawn.h"

Game::Game(){
    int H = ChessBoard::Chessboard->HEIGHT;
    int W = ChessBoard::Chessboard->WIDTH;
	window.create(sf::VideoMode(H, W), "ChessBoard");

    // rotate the window by 90
    view.setSize(window.getSize().x, window.getSize().y);
    view.setCenter(window.getSize().x / 2.f, window.getSize().y / 2.f);
    view.setRotation(90);

    window.setView(view);
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
            const std::vector<sf::Vector2i>& legalMoves = selectedPiece->legalMoves(dragOrigin);
            sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            sf::FloatRect bounds = selectedPiece->GetSprite().getLocalBounds();
            selectedPiece->GetSprite().setOrigin(bounds.width / 2.f, bounds.height / 2.f);
            selectedPiece->GetSprite().setPosition(pos);

            updatePieceCordinates(e, isDragging, selectedPiece, dragOrigin, legalMoves);

            if (ChessBoard::InBounds(dragOrigin) && ChessBoard::Chessboard->board[dragOrigin.x][dragOrigin.y])
                Game::highLightMoves(selectedPiece, dragOrigin, legalMoves);
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

                    if (selectedPiece->pieceType == ChessPiece::PAWN) {
                        if (auto pawn = dynamic_cast<Pawn*>(selectedPiece)) {
                            pawn->setMoved(true);
                        }
                    }

                    selectedPiece = nullptr;
                    isDragging = false;
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



