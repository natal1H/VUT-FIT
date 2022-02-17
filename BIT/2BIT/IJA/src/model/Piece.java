package model;

import java.util.ArrayList;

/**
 * Piece presents interface for different types of figures in game of chess, that is Pawn, Rook, Horse, Bishop, King and Queen
 *
 * @author Natália Holková (xholko02)
 */
public interface Piece {

    /**
     * Get list of boxes, where piece can move
     * @return List of possible boxes to move to
     */
    public ArrayList<Box> getValidMoveBoxes();

    /**
     * Piece gets captures, therefore is removed from position
     * @return Success or failure in capturing the piece
     */
    public boolean getCaptured();

    /**
     * Returns color of piece
     * @return Piece color
     */
    public Color getColor();

    /**
     * Set position of piece
     * @param position Box where piece will be standing
     */
    public void setPosition(Box position);

    /**
     * Returns box, where piece currently resides
     * @return Box or null
     */
    public Box getPosition();

    /**
     * Move piece to new position if possible.
     * @param newPosition New position of piece
     * @return Move object or null if move not possible
     */
    public Move moveToPosition(Box newPosition);
}
