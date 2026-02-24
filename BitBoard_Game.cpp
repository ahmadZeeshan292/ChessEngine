#pragma once

#include "BitBoard_Game.h"
#include "BitBoard_ChessBoard_visuals.h"
#include <iostream>

Game::Game(ChessBoard_BitBoard* a) {
    int H = a->HEIGHT;
    int W = a->WIDTH;

    window.create(sf::VideoMode(H, W), "ChessBoard");

    // rotate the window by 90
    view.setSize(window.getSize().x, window.getSize().y);
    view.setCenter(window.getSize().x / 2.f, window.getSize().y / 2.f);
    view.setRotation(90);

    window.setView(view);

    updateState = true;
}

void Game::Initialize(ChessBoard_BitBoard* a)
{
    int H = a->HEIGHT;
    int W = a->WIDTH;

    sf::Sprite& whiteTile = a->WhiteTile;
    sf::Sprite& blackTile = a->BlackTile;

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

void Game::Update(ChessBoard_BitBoard* a) {

    sf::Vector2i dragOrigin;
	sf::Sprite* draggingSprite = nullptr;
    bool isDragging = false;
    int pieceType = -1;

    Game::Initialize(a);
    Game::InitializeBoard(a);

    while (window.isOpen()) {

        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed)
                window.close();
        }
        window.clear();

        for (auto& tiles : tileLayout) {
            window.draw(tiles);
        }

        Game::PieceDragLogic(a, e, pieceType, isDragging, dragOrigin);

        if (isDragging) {

			uint64_t legalMoves = a->Board.LegalMoves(0x00ULL, dragOrigin.x * 8 + dragOrigin.y, pieceType, idx(a->player), false);
            /*if (selectedPiece->color == ChessBoard::Chessboard->player) {
                const std::vector<sf::Vector2i>& legalMoves = selectedPiece->legalMoves(dragOrigin, GameState::PLAYER);*/

                updatePieceCordinates(a, e, pieceType, draggingSprite, isDragging, dragOrigin, legalMoves);

                /*if (ChessBoard::InBounds(dragOrigin) && ChessBoard::Chessboard->board[dragOrigin.x][dragOrigin.y])
                    Game::highLightMoves(selectedPiece, dragOrigin, legalMoves);
            }*/
        }

        /*Game::UpdatePieces();*/
        Game::UpdatePieces(a);
        window.display();
        if (a->Board.Terminal()) break;
    }
}

void Game::updatePieceCordinates(ChessBoard_BitBoard* a, sf::Event& event, int& selectedPiece, sf::Sprite*& draggingSprite, bool& isDragging, sf::Vector2i& dragOrigin, uint64_t legalMoves)
{
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    float W = a->WIDTH / 8.0f;
    float H = a->HEIGHT / 8.0f;

    int mapTypeIdx = selectedPiece + (a->player == Turn::BLACK ? 6 : 0);
    int oldBitIdx = dragOrigin.x * 8 + dragOrigin.y;

    unordered_map<uint64_t, Sprite_Table*>& pieceMap = a->spriteBitBoardMap[mapTypeIdx].sprite_map;

    if (!draggingSprite) {
        if (pieceMap.find(oldBitIdx) != pieceMap.end()) {
            draggingSprite = &pieceMap[oldBitIdx]->sprite;
        }
        else {
            return;
        }
    }
    sf::FloatRect bounds = draggingSprite->getLocalBounds();
    draggingSprite->setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    draggingSprite->setPosition(mousePos);

    if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {

        if (isDragging && draggingSprite && selectedPiece >= 0) {
            sf::Vector2i dropIndex(mousePos.x / W, mousePos.y / H);
            int newBitIdx = dropIndex.x * 8 + dropIndex.y;

            uint64_t fromMask = 1ULL << oldBitIdx;
            uint64_t toMask = 1ULL << newBitIdx;

            if (legalMoves & toMask) {
                if (dropIndex != dragOrigin) {

                    pair<uint8_t, uint8_t> vs = a->Board.makeMove(fromMask, toMask, selectedPiece, idx(a->player));

                    if (vs != pair<uint8_t, uint8_t>{-1, -1}) {
                        cout << "PIECE MAPPING REMOVED PIECETYPE " <<  vs.first << " BITPOS " << vs.second << endl;
						auto m = a->spriteBitBoardMap[vs.first].sprite_map;
                        if (m.find(vs.second) != m.end()) {
                            cout << "MAPPING FOUND" << endl;
                        }
                        else {
							cout << "MAPPING NOT FOUND" << endl;
                        }
						a->spriteBitBoardMap[vs.first].sprite_map.erase(vs.second);
                    }

                    pieceMap[newBitIdx] = std::move(pieceMap[oldBitIdx]);
                    pieceMap.erase(oldBitIdx);

                    cout << "new BitIdx = " << newBitIdx << endl;
                    cout << "old BitIdx = " << oldBitIdx << endl;

                    a->player = a->player == Turn::WHITE ? Turn::BLACK : Turn::WHITE;
                }

                draggingSprite->setOrigin(0, 0);
                draggingSprite->setPosition((dropIndex.x + 1) * W, dropIndex.y * H);

                cout << "Drop Index = " << dropIndex.x << ',' << dropIndex.y << endl;

                std::cout << "Mouse Dropped" << endl;
                isDragging = false;
                draggingSprite = nullptr;
                selectedPiece = -1;
            }
            else {
                if (dropIndex != dragOrigin) {
                    cout << "ILLEGAL MOVE BEING MADE MOVE RESET!" << endl;
                }
                draggingSprite->setOrigin(0, 0);
                draggingSprite->setPosition((dragOrigin.x + 1) * W, dragOrigin.y * H);

                cout << "Drop Index = " << dropIndex.x << ',' << dropIndex.y << endl;

                std::cout << "Mouse Dropped" << endl;
                isDragging = false;
                draggingSprite = nullptr;
                selectedPiece = -1;
            }
        }
    }

}

void Game::PieceDragLogic(ChessBoard_BitBoard* a, sf::Event& event, int& selectedPieceType, bool& isDragging,  sf::Vector2i& dragOrigin)
{
    // Mouse drag: select the piece
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {

        sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        int H = a->HEIGHT / 8;
        int W = a->WIDTH / 8;

        dragOrigin = sf::Vector2i((pos.x) / H, pos.y / W);

        // Check if the piece being dragged is valid and belongs to the players whose turn is due
		bool selectedPiece = ((a->Board.Boards[idx(a->player)][6])->board >> (dragOrigin.x * 8 + dragOrigin.y)) & 1ULL;

        if (selectedPiece) {
            isDragging = true;
            cout << "Valid Piece is being dragged" << endl;
            cout << "Drag Orgins = " << dragOrigin.x << ',' << dragOrigin.y << endl;

            for (int piece = 0; piece < 6; piece++) {
                if ((a->Board.Boards[idx(a->player)][piece]->board >> (dragOrigin.x * 8 + dragOrigin.y) & 1ULL)) {
                    selectedPieceType = piece;
                }
            }
        }
        else {
			cout << "No valid piece at (" << dragOrigin.x << ',' << dragOrigin.y << ")" << " bit position (" << dragOrigin.x * 8 + dragOrigin.y << ')' << endl;
        }
    }
}

void Game::InitializeBoard(ChessBoard_BitBoard* a)
{
    // item.first = color + pieceType
    // map.first = bit position

	int H = a->HEIGHT / 8;
	int W = a->WIDTH / 8;

    auto map = a->spriteBitBoardMap;

    for (auto const& item : map) {
		auto sprite_mapping = item.second.sprite_map;
        for (auto const& map : sprite_mapping) {
            sf::Sprite& cur = map.second->sprite;

			int bitPosition = map.first;

			int rank = bitPosition / 8;
			int file = bitPosition % 8;

			int px = (file) * W;
			int py = (rank + 1) * H;

			cur.setPosition(py, px);
			cur.setRotation(90);
			window.draw(cur);
        }
    }
}

void Game::UpdatePieces(ChessBoard_BitBoard* a)
{
    int H = a->HEIGHT / 8;
    int W = a->WIDTH / 8;

    auto map = a->spriteBitBoardMap;

    for (auto const& item : map) {
        auto sprite_mapping = item.second.sprite_map;
        for (auto const& map : sprite_mapping) {
            sf::Sprite& cur = map.second->sprite;

            int bitPosition = map.first;

            int rank = bitPosition  / 8;
            int file = bitPosition % 8;

            int px = (file) * W;
            int py = (rank + 1) * H;

            window.draw(cur);

            // item.first = color + pieceType
            // map.first = bit position
        }
    }
}


//void Game::highLightMoves(Piece* selectedPiece, sf::Vector2i& dragOrigin, const vector<sf::Vector2i>& legalMoves)
//{
//    if (!selectedPiece) return;
//    cout << dragOrigin.x << ',' << dragOrigin.y << endl;
//
//    int tileWidth = ChessBoard::Chessboard->WIDTH / 8;
//    int tileHeight = ChessBoard::Chessboard->HEIGHT / 8;
//
//    if (legalMoves.empty())
//        return;
//
//    std::cout << "FOUND MOVES!!!" << std::endl;
//    sf::Sprite highlightTile = ChessBoard::Chessboard->highlightTile;
//
//    for (const auto& move : legalMoves) {
//        highlightTile.setPosition(move.x * tileWidth, move.y * tileHeight);
//        window.draw(highlightTile);
//    }
//}



