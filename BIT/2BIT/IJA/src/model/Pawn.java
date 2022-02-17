package model;

import java.util.ArrayList;


/**
 * Pawn is one of classes implementing Piece interface. It can only move forward one square or two squares in case
 * it has not moved before. It can captured enemy pieces that are one square in front of it in diagonal direction.
 *
 * @author Natália Holková (xholko02)
 */
public class Pawn implements Piece {
    private Color color;
    private Box position;
    private int wasMoved;

    /**
     * Initialize Pawn object. Sets color, position and wasMoved attribute.
     * @param color Color of piece
     */
    public Pawn(Color color) {
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
        // Valid moves include those moves, where piece could get captured

        ArrayList<Box> possibleBoxes = new ArrayList<Box>();

        // Check if box in front is empty - direction depends on color of pawn
        Box next = position.getNextBox((color == Color.WHITE) ? Box.Direction.U : Box.Direction.D); // Will crash here is position is null
        if (next != null && next.getPiece() == null) { // Next box exists and is empty
            possibleBoxes.add(next); // Box in front is empty, add it to the list

            if (wasMoved == 0) { // Pawn was not moved before, could also move to next box
                next = next.getNextBox((color == Color.WHITE) ? Box.Direction.U : Box.Direction.D);
                if (next != null && next.getPiece() == null)
                    possibleBoxes.add(next); // Box in front is empty, add it to the list
            }
        }

        // Check if boxes in LU, RU (case WHITE pawn) or in LD, RD (BLACK pawn) direction are valid moves (have enemy figure on them
        if (color == Color.WHITE) {
            next = position.getNextBox(Box.Direction.LU);
            if (next != null && next.getPiece() != null && next.getPiece().getColor() == Color.BLACK) // Can capture BLACK piece in LU
                possibleBoxes.add(next);
            next = position.getNextBox(Box.Direction.RU);
            if (next != null && next.getPiece() != null && next.getPiece().getColor() == Color.BLACK) // Can capture BLACK piece in RU
                possibleBoxes.add(next);
        }
        else {
            next = position.getNextBox(Box.Direction.LD);
            if (next != null && next.getPiece() != null && next.getPiece().getColor() == Color.WHITE) // Can capture WHITE piece in LD
                possibleBoxes.add(next);
            next = position.getNextBox(Box.Direction.RD);
            if (next != null && next.getPiece() != null && next.getPiece().getColor() == Color.WHITE) // Can capture WHITE piece in RD
                possibleBoxes.add(next);
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
/*
    private void checkIfPromote() {
        if (this.position.getRow() == 2 || this.position.getRow() == 7){
            System.out.println("Caaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
            //TODO popup choice of figure, until then Queen
            Piece piece = new Queen(this.position.getPiece().getColor());
            Box pos = this.position;
            this.position = null;
            piece.setPosition(pos);
            pawnPromotion(piece);
        }
    }*//*
    public void promotePawnSet(Box box){
        Piece piece = box.getPiece();
        piece = piece.pawnPromotionRAA();
        boxes[box.getRow()][box.getCol()].setPiece((Queen) piece);
        ((Queen) piece).setPosition(boxes[0][3]);
    }
                                game.getBoard().getPieceSet(model.Color.WHITE).removePiece(captured);
*//*
    //TODO~##############################################################################
    /**
     * Remove piece from list of pieces currently on board
     * @param piece Piece, which will be removed
     * @param color Color of piece set
     */
  /*  public Piece pawnPromotionRAA(Piece piece) {
        Color color = piece.getColor();
        pieces.remove(piece);
        piece = new Queen(color);
        pieces.add(piece);
        return piece;
    }
*/
}
