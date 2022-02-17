package model;

import java.util.ArrayList;

/**
 * Horse one of classes implementing Piece interface. It moves in shape of an L and can jump over pieces in the way.
 *
 * @author Natália Holková (xholko02)
 */
public class Horse implements Piece {
    private Color color;
    private Box position;

    /**
     * Initialize Horse object. Sets color and position.
     * @param color Color of piece
     */
    public Horse(Color color) {
        this.color = color;
        this.position = null;
    }

    /**
     * Get list of boxes, where piece can move
     * @return List of possible boxes to move to
     */
    public ArrayList<Box> getValidMoveBoxes() {
        // Expects that piece is on board - will crash if not
        // Valid moves include those moves, where piece could get captured

        ArrayList<Box> possibleBoxes = new ArrayList<Box>();

        Box next = position.getNextBox(Box.Direction.U);
        if (next != null) {
            next = next.getNextBox(Box.Direction.U);
            if (next != null) {
                // Test L and R
                Box left = next.getNextBox(Box.Direction.L);
                if (left != null && (left.getPiece() == null || left.getPiece().getColor() != this.color))
                    possibleBoxes.add(left);
                Box right = next.getNextBox(Box.Direction.R);
                if (right != null && (right.getPiece() == null || right.getPiece().getColor() != this.color))
                    possibleBoxes.add(right);
            }
        }

        next = position.getNextBox(Box.Direction.L);
        if (next != null) {
            next = next.getNextBox(Box.Direction.L);
            if (next != null) {
                // Test U and D
                Box up = next.getNextBox(Box.Direction.U);
                if (up != null && (up.getPiece() == null || up.getPiece().getColor() != this.color))
                    possibleBoxes.add(up);
                Box down = next.getNextBox(Box.Direction.D);
                if (down != null && (down.getPiece() == null || down.getPiece().getColor() != this.color))
                    possibleBoxes.add(down);
            }
        }

        next = position.getNextBox(Box.Direction.D);
        if (next != null) {
            next = next.getNextBox(Box.Direction.D);
            if (next != null) {
                // Test L and R
                Box left = next.getNextBox(Box.Direction.L);
                if (left != null && (left.getPiece() == null || left.getPiece().getColor() != this.color))
                    possibleBoxes.add(left);
                Box right = next.getNextBox(Box.Direction.R);
                if (right != null && (right.getPiece() == null || right.getPiece().getColor() != this.color))
                    possibleBoxes.add(right);
            }
        }

        next = position.getNextBox(Box.Direction.R);
        if (next != null) {
            next = next.getNextBox(Box.Direction.R);
            if (next != null) {
                // Test U and D
                Box up = next.getNextBox(Box.Direction.U);
                if (up != null && (up.getPiece() == null || up.getPiece().getColor() != this.color))
                    possibleBoxes.add(up);
                Box down = next.getNextBox(Box.Direction.D);
                if (down != null && (down.getPiece() == null || down.getPiece().getColor() != this.color))
                    possibleBoxes.add(down);
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
        if (position == null) return false; // Pawn is not on board, therefore cannot be captured

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

            return move;
        }
        else // Not valid move
            return null;
    }
}
