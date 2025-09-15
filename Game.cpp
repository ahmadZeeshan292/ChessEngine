#pragma once

#include "Game.h"
#include "ChessBoard.h"
#include <iostream>
#include <exception>

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

        Game::updatePieceCordinates(e, isDragging, selectedPiece, dragOrigin);

        if (isDragging && selectedPiece) {
            sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            selectedPiece->GetSprite().setPosition(pos);
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

void Game::updatePieceCordinates(sf::Event& event, bool& isDragging, Piece*& selectedPiece, sf::Vector2i dragOrigin)
{
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

            // Update board
            ChessBoard::Chessboard->board[dropIndex.x][dropIndex.y] = selectedPiece;
            ChessBoard::Chessboard->board[dragOrigin.x][dragOrigin.y] = nullptr;

            selectedPiece = nullptr;
            isDragging = false;
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

