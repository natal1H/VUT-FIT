package model;

import java.util.ArrayList;

/**
 * King is one of classes implementing Piece interface. It can move one square one square in any direction.
 * However, it CANNOT move to square, where it could be captured (cannot be sacrificed).
 *
 * @author Natália Holková (xholko02)
 */
public class King implements Piece {
    private Color color;
    private Box position;
    private int wasMoved;

    /**
     * Initialize King object. Sets color, position and wasMoved attribute.
     * @param color Color of piece
     */
    public King(Color color) {
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
            // Check direction dir
            Box next = position.getNextBox(dir);
            if (next != null && next.getPiece() == null) {
                // Remove king from board temporarily
                position.setPiece(null);
                if (!next.isInDanger((color == Color.WHITE) ? Color.BLACK : Color.WHITE))
                    possibleBoxes.add(next);
                position.setPiece(this);
            }
            else if (next != null && next.getPiece() != null && next.getPiece().getColor() != color) {
                // Remove king from board temporarily
                position.setPiece(null);
                if (!next.isInDanger((color == Color.WHITE) ? Color.BLACK : Color.WHITE))
                    possibleBoxes.add(next);
                position.setPiece(this);
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

    /**
     *
     * @return
     */
    public boolean isCheck() {
        // Check is he is endangered on its current position
        if (position.isInDanger((color == Color.WHITE) ? Color.BLACK : Color.WHITE)) {
            // Check if he still has some possible moves
            ArrayList<Box> validMoves = getValidMoveBoxes(); // Boxes he can move to
            if (validMoves.size() == 0) {
                // Cannot move anywhere, therefore it is CHECKMATE, not just check
                return false;
            }
            else {
                for (Box moveTo : validMoves) {
                    if (!moveTo.isInDanger((color == Color.WHITE) ? Color.BLACK : Color.WHITE)) {
                        // Can move to this square without getting captured - therefore is it CHECK (he can still escape)
                        return true;
                    }
                }
            }
            return false;
        }
        else
            return false;
    }

    /**
     *
     * @return
     */
    public boolean isCheckmate() {
        // Check is he is endangered on its current position
        if (position.isInDanger((color == Color.WHITE) ? Color.BLACK : Color.WHITE)) {
            // Check if he still has some possible moves
            ArrayList<Box> validMoves = getValidMoveBoxes(); // Boxes he can move to
            if (validMoves.size() == 0) {
                // Cannot move anywhere, therefore it is CHECKMATE
                return true;
            }
            else {
                for (Box moveTo : validMoves) {
                    if (!moveTo.isInDanger((color == Color.WHITE) ? Color.BLACK : Color.WHITE)) {
                        // Can move to this square without getting captured - therefore is it CHECK
                        return false;
                    }
                }
            }
            return true;
        }
        else
            return false;
    }

    /**
     * Do not use on it's own - only Game calls this one
     * @return
     */
    public boolean isStalemate() {
        // Check is he is endangered on its current position
        if (!position.isInDanger((color == Color.WHITE) ? Color.BLACK : Color.WHITE)) {
            // Check if he still has some possible moves
            ArrayList<Box> validMoves = getValidMoveBoxes(); // Boxes he can move to
            if (validMoves.size() == 0) {
                // Cannot move anywhere, therefore it is STALEMATE (he is not in danger)
                return true;
            }
            else {
                for (Box moveTo : validMoves) {
                    if (!moveTo.isInDanger((color == Color.WHITE) ? Color.BLACK : Color.WHITE)) {
                        // Can move to this square without getting captured - therefore is it not STALEMATE
                        return false;
                    }
                }
            }
            // Cannot move anywhere safely - STALEMATE
            return true;
        }
        else
            return false;
    }

    public void descWasMoved() {
        wasMoved--;
    }

    public void incWasMoved() {
        wasMoved++;
    }
}
