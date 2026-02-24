#include "Pieces.h"
#include "ChessBoard.h"

Piece::Piece(string path, Turn color_, ChessPiece type_){
	pieceTexture.loadFromFile(path);

	pieceSprite.setTexture(pieceTexture);
	pieceSprite.setScale(
		float(ChessBoard::Chessboard->WIDTH / 8) / pieceTexture.getSize().x,
		float(ChessBoard::Chessboard->HEIGHT / 8) / pieceTexture.getSize().y
	);

	color = color_;
	pieceType = type_;
	pinnedPiece = sf::Vector2i();
}

Piece::Piece(const Piece& piece) : pieceSprite(piece.pieceSprite), pieceTexture(piece.pieceTexture), Coordinates(piece.Coordinates), color(piece.color), pieceType(piece.pieceType), PinningPiece(piece.PinningPiece)
{
	pieceSprite.setTexture(pieceTexture);
}

