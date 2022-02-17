package model;

import java.util.ArrayList;

/**
 * Board represents chess board in real life. It has (usually) 8x8 grid of boxes, where pieces can be placed and two sets of pieces.
 *
 * @author Natália Holková (xholko02)
 */
public class Board {
    public final int SIZE = 8; // Size of chess board

    private Box[][] boxes;
    private PieceSet whitePieceSet;
    private PieceSet blackPieceSet;

    /**
     * Initialize Board object. Creates 2D array of {@link Box Box} objects. Set neighbouring boxes for each box.
     * Creates black and white {@link PieceSet PieceSet} objects.
     */
    public Board() {
        System.out.println("Creating Board object...");

        // Create all boxes on board
        boxes = new Box[SIZE][SIZE];
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                boxes[row][col] = new Box(row, col); // Create singular box
            }
        }

        // Add neighbouring boxes to each box
        for (int row = 0; row < SIZE; row++) {
            for (int col = 0; col < SIZE; col++) {
                Box tmp = boxes[row][col];

                tmp.addNextBox(Box.Direction.D, getBox(row - 1, col)); // D
                tmp.addNextBox(Box.Direction.U, getBox(row + 1, col)); // U
                tmp.addNextBox(Box.Direction.L, getBox(row, col - 1)); // L
                tmp.addNextBox(Box.Direction.R, getBox(row, col + 1)); // R
                tmp.addNextBox(Box.Direction.LD, getBox(row - 1, col - 1)); // LD
                tmp.addNextBox(Box.Direction.LU, getBox(row + 1, col - 1)); // LU
                tmp.addNextBox(Box.Direction.RD, getBox(row - 1, col + 1)); // RD
                tmp.addNextBox(Box.Direction.RU, getBox(row + 1, col + 1)); // RU
            }
        }

        // Create white and black piece set;
        whitePieceSet = new PieceSet(Color.WHITE);
        blackPieceSet = new PieceSet(Color.BLACK);

        // Set white and black pieces on board
        setPieceSets();
    }

    /**
     * Returns box at position or null if out of bounds.
     * @param row Row in 2D array
     * @param col Column in 2D array
     * @return Box object or null
     */
    public Box getBox(int row, int col) {
        if (row < 0 || row >= this.SIZE || col < 0 || col >= this.SIZE)
            return null;
        else
            return boxes[row][col];
    }

    /**
     * Returns piece set of desired color.
     * @param color Color of piece set
     * @return PieceSet object
     */
    public PieceSet getPieceSet(Color color) {
        return (color == Color.WHITE) ? whitePieceSet : blackPieceSet;
    }

    /**
     * Places pieces from both created piece sets on board.
     * To be called only at start of game when no pieces were yet captured or moved.
     */
    private void setPieceSets() {
        System.out.println("Setting pieces on board...");

        // Set white piece set
        int pawnsSet = 0, bishopsSet = 0, rooksSet = 0, horsesSet = 0;
        ArrayList<Piece> pieces = whitePieceSet.getPieces();
        for (Piece piece : pieces) { // Iterate through pieces
            if (piece instanceof Pawn)  {
                boxes[1][pawnsSet].setPiece((Pawn) piece);
                ((Pawn) piece).setPosition(boxes[1][pawnsSet++]);
            }
            else if (piece instanceof King) {
                boxes[0][4].setPiece((King) piece);
                ((King) piece).setPosition(boxes[0][4]);
            }
            else if (piece instanceof Queen) {
                boxes[0][3].setPiece((Queen) piece);
                ((Queen) piece).setPosition(boxes[0][3]);
            }
            else if (piece instanceof Bishop) {
                boxes[0][2 + 3*bishopsSet].setPiece((Bishop) piece);
                ((Bishop) piece).setPosition(boxes[0][2 + 3*(bishopsSet++)]);
            }
            else if (piece instanceof Horse) {
                boxes[0][1 + 5*horsesSet].setPiece((Horse) piece);
                ((Horse) piece).setPosition(boxes[0][1 + 5*(horsesSet++)]);
            }
            else if (piece instanceof Rook) {
                boxes[0][7*rooksSet].setPiece((Rook) piece);
                ((Rook) piece).setPosition(boxes[0][7*(rooksSet++)]);
            }
        }

        // Set black piece set
        pawnsSet = 0; bishopsSet = 0; rooksSet = 0; horsesSet = 0;
        pieces = blackPieceSet.getPieces();
        for (Piece piece : pieces) { // Iterate through pieces
            if (piece instanceof Pawn) {
                boxes[6][pawnsSet].setPiece((Pawn) piece);
                ((Pawn) piece).setPosition(boxes[6][pawnsSet++]);
            }
            else if (piece instanceof King) {
                boxes[7][4].setPiece((King) piece);
                ((King) piece).setPosition(boxes[7][4]);
            }
            else if (piece instanceof Queen) {
                boxes[7][3].setPiece((Queen) piece);
                ((Queen) piece).setPosition(boxes[7][3]);
            }
            else if (piece instanceof Bishop) {
                boxes[7][2 + 3*bishopsSet].setPiece((Bishop) piece);
                ((Bishop) piece).setPosition(boxes[7][2 + 3*(bishopsSet++)]);
            }
            else if (piece instanceof Horse) {
                boxes[7][1 + 5*horsesSet].setPiece((Horse) piece);
                ((Horse) piece).setPosition(boxes[7][1 + 5*(horsesSet++)]);
            }
            else if (piece instanceof Rook) {
                boxes[7][7*rooksSet].setPiece((Rook) piece);
                ((Rook) piece).setPosition(boxes[7][7*(rooksSet++)]);
            }
        }
    }
}
