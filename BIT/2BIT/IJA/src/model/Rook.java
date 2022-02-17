package model;

import java.util.ArrayList;

/**
 * Rook one of classes implementing Piece interface. Unlike other pieces, it can only move horizontally and vertically.
 *
 * @author Natália Holková (xholko02)
 */
public class Rook implements Piece {
    private Color color;
    private Box position;
    private int wasMoved;

    /**
     * Initialize Rook object. Set color, position and wasMoved attribute
     * @param color Color of piece
     */
    public Rook(Color color) {
        this.color = color;
        this.position = null;
        this.wasMoved = 0;
    }

    /**
     * Get list of boxes, where piece can move
     * @return List of possible boxes to move to
     */
    public ArrayList<Box> getValidMoveBoxes() {
        // Expects that piece is on board - will crash if not
        // Valid moves include those moves, where this piece could get captured

        ArrayList<Box> possibleBoxes = new ArrayList<Box>();

        for (Box.Direction dir : Box.Direction.values()) {
            if (dir == Box.Direction.LD || dir == Box.Direction.LU || dir == Box.Direction.RD || dir == Box.Direction.RU)
                continue;
            // Check possible moves in dir direction
            Box next = position.getNextBox(dir);
            while (next != null) {
                // Check if box is empty or has enemy piece on it
                if (next.getPiece() == null) {
                    possibleBoxes.add(next);
                    next = next.getNextBox(dir);
                }
                else if (color != next.getPiece().getColor()) {
                    possibleBoxes.add(next);
                    break;
                }
                else
                    break;
            }
        }

        return possibleBoxes;
    }

    /**
     * Piece gets captures, therefore is removed from position
     * @return Success or failure in capturing the piece
     */
    public boolean getCaptured() {
        // Note: PieceSet has to remove piece from active pieces and add him to captured pieces
        if (position == null) return false; // Rook is not on board, therefore cannot be captured

        position.setPiece(null); // Remove piece from box
        position = null; // Remove box from piece
        return true;
    }

    /**
     * Set position of piece
     * @param position Box where piece will be standing
     */
    public void setPosition(Box position) {
        this.position = position;
    }

    /**
     * Returns box, where piece currently resides
     * @return Box or null
     */
    public Box getPosition() {
        return position;
    }

    /**
     * Returns color of piece
     * @return Piece color
     */
    public Color getColor() {
        return color;
    }

    /**
     * Move piece to new position if possible.
     * @param newPosition New position of piece
     * @return Move object or null if move not possible
     */
    public Move moveToPosition(Box newPosition) {
        ArrayList<Box> validPositions = getValidMoveBoxes();
        if (validPositions.contains(newPosition)) {
            Box oldPosition = position; // Copy old position for Move object

            // Find out if any piece was captured
            Piece capturedPiece = newPosition.getPiece();
            if (!(capturedPiece != null && color != capturedPiece.getColor()))
                capturedPiece = null;
            else
                capturedPiece.setPosition(null);

            // Change positions
            position.setPiece(null); // Remove piece from old box
            position = newPosition; // Set new position where pawn moved to
            position.setPiece(this); // Set piece on new position

            // Create new move object
            Move move = new Move(oldPosition, newPosition, this, capturedPiece);

            // Change was moved to true
            wasMoved +=1;

            return move;
        }
        else // Not valid move
            return null;
    }

    public void descWasMoved() {
        wasMoved--;
    }

    public void incWasMoved() {
        wasMoved++;
    }
}
