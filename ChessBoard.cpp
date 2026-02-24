
#include "ChessBoard.h"

ChessBoard::ChessBoard() : HEIGHT(800), WIDTH(800), board(8, vector<Piece*>(8, nullptr)) {
	Chessboard = this;

    blackTileTexture.loadFromFile("Images/square gray dark _2x.png");
    whiteTileTexture.loadFromFile("Images/square gray light _2x.png");
    highlightTileTexture.loadFromFile("Images/square brown light_2x.png");

    BlackTile.setTexture(blackTileTexture);
    WhiteTile.setTexture(whiteTileTexture);
    highlightTile.setTexture(highlightTileTexture);

    BlackTile.setScale(
        float(WIDTH / 8) / blackTileTexture.getSize().x,
        float(HEIGHT / 8) / blackTileTexture.getSize().y
    );

    WhiteTile.setScale(
        float(WIDTH / 8) / whiteTileTexture.getSize().x,
        float(HEIGHT / 8) / whiteTileTexture.getSize().y
    );

    highlightTile.setScale(
        float(WIDTH / 8) / whiteTileTexture.getSize().x,
        float(HEIGHT / 8) / whiteTileTexture.getSize().y
    );

    loadDefaultPosition();
    player = Turn::WHITE;
}

void ChessBoard::InitializeBoard()
{
    
    board[0][5] = new Rook("Images/w_rook.png", Turn::WHITE);
    //board[0][1] = new Knight("Images/w_knight.png", Turn::WHITE);
    //board[0][2] = new Bishop("Images/w_bishop.png", Turn::WHITE);
    board[0][2] = new Queen("Images/w_queen.png", Turn::WHITE);
    board[0][7] = new King("Images/w_king.png", Turn::WHITE);
    board[0][0] = new Bishop("Images/w_bishop.png", Turn::WHITE);
    //board[0][6] = new Knight("Images/w_knight.png", Turn::WHITE);
    //board[1][6] = new Pawn("Images/w_pawn.png", Turn::WHITE);

    //board[1][2] = new Bishop("Images/w_bishop.png", Turn::WHITE);

    board[1][0] = new Pawn("Images/w_pawn.png", Turn::WHITE);

    for (int i = 0;i < 8; i++) {
        //board[1][i] = new Pawn("Images/w_pawn.png", Turn::WHITE);
    }

    //board[7][0] = new Bishop("Images/b_bishop.png", Turn::BLACK);
    board[1][4] = new Knight("Images/b_knight.png", Turn::BLACK);
    //board[7][2] = new Bishop("Images/b_bishop.png", Turn::BLACK);
    //board[7][3] = new Queen("Images/b_Queen.png", Turn::BLACK);
    board[7][4] = new King("Images/b_king.png", Turn::BLACK);
    //board[7][5] = new Bishop("Images/b_bishop.png", Turn::BLACK);
    //board[7][6] = new Knight("Images/b_knight.png", Turn::BLACK);
    board[6][4] = new Pawn("Images/b_pawn.png", Turn::BLACK);

    //board[7][6] = new Rook("Images/b_rook.png", Turn::BLACK);
    
    board[4][1] = new Pawn("Images/b_pawn.png", Turn::BLACK);
    for (int i = 0;i < 8; i++) {
        //board[6][i] = new Pawn("Images/b_pawn.png", Turn::BLACK);
    }
    
    WhiteKing = std::pair<King*, sf::Vector2i>(dynamic_cast<King*>(board[0][7]), sf::Vector2i({0, 7}));
    BlackKing = std::pair<King*, sf::Vector2i>(dynamic_cast<King*>(board[7][4]), sf::Vector2i({7, 4}));
    
}

bool ChessBoard::InBounds(sf::Vector2i coordinates)
{
    return (coordinates.x >= 0 && coordinates.x < 8 && coordinates.y >= 0 && coordinates.y < 8);
}

Turn ChessBoard::Player()
{
    return player;
}

vector<sf::Vector2i> ChessBoard::GenerateAllLegalMoves(Turn color_, GameState state)
{
    vector<sf::Vector2i> ans;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] && board[i][j]->getColor() == color_) {

                vector<sf::Vector2i> pieceMoves;

                if (board[i][j]->pieceType == ChessPiece::KING) {
                     pieceMoves = MoveGenerator::KingPossibleMoves(sf::Vector2i({ i, j }));
                }
                else if (board[i][j]->pieceType == ChessPiece::PAWN) {
                     pieceMoves = MoveGenerator::PawnAttackMoves(sf::Vector2i({ i, j }));
                }
                else {
                     pieceMoves = board[i][j]->legalMoves(sf::Vector2i({ i, j }), GameState::GAME);
                }
                for (auto& move : pieceMoves) {
                    if (find(ans.begin(), ans.end(), move) == ans.end()) {
                        ans.push_back(move);
                    }
                }
            }
        }
    }
    return ans;
}

bool  ChessBoard::KinginPath(sf::Vector2i sourcePiece, sf::Vector2i destinationPiece, sf::Vector2i placement)
{
        if (placement.x == 0 && placement.y == 0) return sourcePiece == destinationPiece; // no movement

        if (placement.x == 0) {
            return destinationPiece.x == sourcePiece.x &&
                (destinationPiece.y - sourcePiece.y) % placement.y == 0 &&
                (destinationPiece.y - sourcePiece.y) / placement.y >= 0;
        }

        if (placement.y == 0) {
            return destinationPiece.y == sourcePiece.y &&
                (destinationPiece.x - sourcePiece.x) % placement.x == 0 &&
                (destinationPiece.x - sourcePiece.x) / placement.x >= 0;
        }

        // both nonzero
        if ((sourcePiece.x - sourcePiece.x) % placement.x != 0) return false;
        if ((sourcePiece.y - sourcePiece.y) % placement.y != 0) return false;

        int kx = (destinationPiece.x - sourcePiece.x) / placement.x;
        int ky = (destinationPiece.y - sourcePiece.y) / placement.y;

        return (kx == ky && kx >= 0);
}

string ChessBoard::getBoardState()
{
    vector<string> state;

	map<ChessPiece, char> mapp = {
		{ChessPiece::KING, 'K'},
		{ChessPiece::QUEEN, 'Q'},
		{ChessPiece::ROOK, 'R'},
		{ChessPiece::BISHOP, 'B'},
		{ChessPiece::KNIGHT, 'N'},
		{ChessPiece::PAWN, 'P'}
	};

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j]) {
				char playerChar = board[i][j]->getColor() == Turn::WHITE ? 'w' : 'b';
                string pieceState = { playerChar, mapp[board[i][j]->pieceType], char(int('a') + j), char(int('0') + i) };
                state.push_back(pieceState);
			}
		}
	}

	sort(state.begin(), state.end());

	string finalState = "";
	for (auto& s : state) {
		finalState += s + " ";
	}
	return finalState;
}

void ChessBoard::loadDefaultPosition()
{
    board[0][0] = new Rook("Images/w_rook.png", Turn::WHITE);
    board[0][1] = new Knight("Images/w_knight.png", Turn::WHITE);
    board[0][2] = new Bishop("Images/w_bishop.png", Turn::WHITE);
    board[0][3] = new Queen("Images/w_queen.png", Turn::WHITE);
    board[0][4] = new King("Images/w_king.png", Turn::WHITE);
    board[0][5] = new Bishop("Images/w_bishop.png", Turn::WHITE);
    board[0][6] = new Knight("Images/w_knight.png", Turn::WHITE);
    board[0][7] = new Rook("Images/w_rook.png", Turn::WHITE);

    for (int i = 0;i < 8; i++) {
        board[1][i] = new Pawn("Images/w_pawn.png", Turn::WHITE);
    }

    board[7][0] = new Rook("Images/b_rook.png", Turn::BLACK);
    board[7][1] = new Knight("Images/b_knight.png", Turn::BLACK);
    board[7][2] = new Bishop("Images/b_bishop.png", Turn::BLACK);
    board[7][3] = new Queen("Images/b_Queen.png", Turn::BLACK);
    board[7][4] = new King("Images/b_king.png", Turn::BLACK);
    board[7][5] = new Bishop("Images/b_bishop.png", Turn::BLACK);
    board[7][6] = new Knight("Images/b_knight.png", Turn::BLACK);
    board[7][7] = new Rook("Images/b_rook.png", Turn::BLACK);

    for (int i = 0;i < 8; i++) {
        board[6][i] = new Pawn("Images/b_pawn.png", Turn::BLACK);
    }

    WhiteKing = std::pair<King*, sf::Vector2i>(dynamic_cast<King*>(board[0][4]), sf::Vector2i({ 0, 7 }));
    BlackKing = std::pair<King*, sf::Vector2i>(dynamic_cast<King*>(board[7][4]), sf::Vector2i({ 7, 4 }));
}



ChessBoard* ChessBoard::Chessboard = nullptr;