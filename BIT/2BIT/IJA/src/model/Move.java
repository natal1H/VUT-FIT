package model;

/**
 * Move is class for easier internal representation of moves made in game.
 * Contantion information from which square was move made, where to, by what piece and some case captured piece.
 *
 * @author Natália Holková (xholko02)
 */
public class Move {
    private Box startBox;
    private Box endBox;
    private Piece movedPiece;
    private Piece capturedPiece;

    /**
     * Initialize Move object.
     * @param startBox Starting square
     * @param endBox End square
     * @param movedPiece Moved piece
     * @param capturedPiece Captured piece or null in case none was captured
     */
    public Move(Box startBox, Box endBox, Piece movedPiece, Piece capturedPiece) {
        this.startBox = startBox;
        this.endBox = endBox;
        this.movedPiece = movedPiece;
        this.capturedPiece = capturedPiece;
    }

    /**
     *
     * @return
     */
    public Piece getCapturedPiece() {
        return capturedPiece;
    }

    /**
     *
     * @return
     */
    public Piece getMovedPiece() {
        return movedPiece;
    }

    /**
     *
     * @return
     */
    public Box getStartBox() {
        return startBox;
    }

    /**
     *
     * @return
     */
    public Box getEndBox() {
        return endBox;
    }

}
