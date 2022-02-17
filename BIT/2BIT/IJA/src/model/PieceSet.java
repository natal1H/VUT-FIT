package model;

import java.util.ArrayList;

/**
 * This class represent collection of pieces of same color, which belong to same player. It track pieces currently on board
 * as well as those that have been captured
 *
 * @author Natália Holková (xholko02)
 */
public class PieceSet {
    private ArrayList<Piece> pieces;
    private ArrayList<Piece> removedPieces;
    private Color setColor;

    /**
     * Initialize PieceSet object. Set color and create all 16 starting pieces
     * @param setColor Color of piece set
     */
    public PieceSet(Color setColor) {
        System.out.println("Creating PieceSet object of color " + ((setColor == Color.WHITE) ? "WHITE" : "BLACK"));

        this.setColor = setColor;
        this.pieces = new ArrayList<Piece>();
        this.removedPieces = new ArrayList<Piece>();

        createPieces();
    }

    /**
     * Create all pieces, but not yet set them on board (that does Board)
     */
    private void createPieces() {
        System.out.println("Creating " + ((setColor == Color.WHITE) ? "WHITE..." : "BLACK...") + " pieces.");

        pieces.add(new King(setColor)); // Create King and add it to the list of pieces
        pieces.add(new Queen(setColor)); // Create Queen and add it to the list of pieces
        pieces.add(new Bishop(setColor)); // Create Bishop and add it to the list of pieces
        pieces.add(new Bishop(setColor)); // Create Bishop and add it to the list of pieces
        pieces.add(new Horse(setColor)); // Create Horse and add it to the list of pieces
        pieces.add(new Horse(setColor)); // Create Horse and add it to the list of pieces
        pieces.add(new Rook(setColor)); // Create Rook and add it to the list of pieces
        pieces.add(new Rook(setColor)); // Create Rook and add it to the list of pieces
        for (int i = 0; i < 8; i++) // Create 8 Pawns
            pieces.add(new Pawn(setColor)); // Create Pawn and add it to the list of pieces
    }

    /**
     * Remove piece from list of pieces currently on board and add it to the list of captured pieces
     * @param piece Piece, which will be removed
     * @return Success of failure of action
     */
    public boolean removePiece(Piece piece) {
        if (!pieces.contains(piece)) // Not in pieces on board
            return false;
        else {
            pieces.remove(piece);
            removedPieces.add(piece);
            return true;
        }
    }

    /**
     * Returns list of pieces currently on board
     * @return List of pieces
     */
    public ArrayList<Piece> getPieces() {
        return pieces;
    }

    /**
     * Return color of piece set
     * @return Color
     */
    public Color getSetColor() {
        return setColor;
    }

    public ArrayList<Piece> getRemovedPieces() {
        return removedPieces;
    }

    public boolean pawnPromotion(Piece piece){
        Color color = piece.getColor();
        pieces.remove(piece);
        pieces.add(new Queen(color));
        return true;
    }

}
