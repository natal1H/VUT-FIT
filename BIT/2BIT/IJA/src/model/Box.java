package model;

import java.util.EnumMap;

/**
 * Box is class representing single field of playing board. Each field knows its position within board and knows who are
 * its immediate neighbouring boxes. It also knows piece standing atop of it.
 *
 * @author Natália Holková (xholko02)
 */
public class Box extends java.lang.Object {

    /**
     * Enumeration representing direction in which could lie the field next to this on
     */
    public static enum Direction {
        D, L, U, R, LD, LU, RD, RU
    }

    private int row;
    private int col;
    private Color boxColor;
    private EnumMap<Direction, Box> nextBoxes;
    private Piece piece;

    /**
     * Initialize Box object. Sets its position and calculates its color based on coordinates.
     * Prepares empty EnumMap for adding neighbouring boxes later by {@link Board#Board() Board()}.
     * @param row Row of box within Board object
     * @param col Column of box within Board object
     */
    public Box(int row, int col) {
        this.row = row; // Set row number of this box
        this.col = col; // Set column number of this box
        this.boxColor = (((row % 2) == 0 && (col % 2) == 0) || ((col % 2) == 1 && (row % 2) == 1)) ? Color.BLACK : Color.WHITE;
        piece = null;
        nextBoxes = new EnumMap<Direction, Box>(Direction.class);
    }

    /**
     * Returns color of the box
     * @return Box color
     */
    public Color getBoxColor() {
        return boxColor;
    }

    /**
     * Returns row coordinate
     * @return Row
     */
    public int getRow() {
        return row;
    }

    /**
     * Returns column coordinate
     * @return Column
     */
    public int getCol() {
        return col;
    }

    /**
     * Adds box to map of neighbouring boxes in specified direction
     * @param dir Direction in which the box is
     * @param box Box
     */
    public void addNextBox(Box.Direction dir, Box box) {
        nextBoxes.put(dir, box);
    }

    /**
     *
     * @param dir
     * @return
     */
    public Box getNextBox(Box.Direction dir) {
        return nextBoxes.get(dir);
    }

    /**
     * Set piece on this box
     * @param piece Piece
     */
    public void setPiece(Piece piece) {
        this.piece = piece;
    }

    /**
     * Retuns piece standing on this box
     * @return Piece or null
     */
    public Piece getPiece() {
        return piece;
    }

    /**
     * Check if piece could be captured on this square by enemy of color enemyColor
     * @param enemyColor Color of enemy pieces
     * @return true if piece could be captured here, false if not
     */
    public boolean isInDanger(Color enemyColor) {
        // Check diagonal, horizontal and vertical lines (covers Rooks, Bishops, Queens)

        // Check danger in D direction
        Box next = nextBoxes.get(Box.Direction.D);
        if (next != null && next.getPiece() != null && next.getPiece().getColor() == enemyColor && next.getPiece() instanceof King)
            return true; // Danger: piece could be captured by enemy king
        while (next != null) {
            if (next.getPiece() != null && next.getPiece().getColor() == enemyColor &&
                    (next.getPiece() instanceof Rook || next.getPiece() instanceof Queen)) {
                return true; // Danger: piece could be captured by Rooks and Queens
            }
            else {
                if (next.getPiece() != null)
                    break;
                next = next.getNextBox(Box.Direction.D);
            }
        }
        // Check danger in U direction
        next = nextBoxes.get(Box.Direction.U);
        if (next != null && next.getPiece() != null && next.getPiece().getColor() == enemyColor && next.getPiece() instanceof King)
            return true; // Danger: piece could be captured by enemy king
        while (next != null) {
            if (next.getPiece() != null && next.getPiece().getColor() == enemyColor &&
                    (next.getPiece() instanceof Rook || next.getPiece() instanceof Queen)) {
                return true; // Danger: piece could be captured by Rooks and Queens
            }
            else {
                if (next.getPiece() != null)
                    break;
                next = next.getNextBox(Box.Direction.U);
            }
        }
        // Check danger in L direction
        next = nextBoxes.get(Box.Direction.L);
        if (next != null && next.getPiece() != null && next.getPiece().getColor() == enemyColor && next.getPiece() instanceof King)
            return true; // Danger: piece could be captured by enemy king
        while (next != null) {
            if (next.getPiece() != null && next.getPiece().getColor() == enemyColor &&
                    (next.getPiece() instanceof Rook || next.getPiece() instanceof Queen)) {
                return true; // Danger: piece could be captured by Rooks and Queens
            }
            else {
                if (next.getPiece() != null)
                    break;
                next = next.getNextBox(Box.Direction.L);
            }
        }
        // Check danger in R direction
        next = nextBoxes.get(Box.Direction.R);
        if (next != null &&next.getPiece() != null && next.getPiece().getColor() == enemyColor && next.getPiece() instanceof King)
            return true; // Danger: piece could be captured by enemy king
        while (next != null) {
            if (next.getPiece() != null && next.getPiece().getColor() == enemyColor &&
                    (next.getPiece() instanceof Rook || next.getPiece() instanceof Queen)) {
                return true; // Danger: piece could be captured by Rooks and Queens
            }
            else {
                if (next.getPiece() != null)
                    break;
                next = next.getNextBox(Box.Direction.R);
            }
        }
        // Check danger in LD direction
        next = nextBoxes.get(Box.Direction.LD);
        if (next != null && next.getPiece() != null && next.getPiece().getColor() == enemyColor && next.getPiece() instanceof King)
            return true; // Danger: piece could be captured by enemy king
        while (next != null) {
            if (next.getPiece() != null && next.getPiece().getColor() == enemyColor &&
                    (next.getPiece() instanceof Bishop || next.getPiece() instanceof Queen)) {
                return true; // Danger: piece could be captured by Bishops and Queens
            }
            else {
                if (next.getPiece() != null)
                    break;
                next = next.getNextBox(Box.Direction.LD);
            }
        }
        // Check danger in LU direction
        next = nextBoxes.get(Box.Direction.LU);
        if (next != null && next.getPiece() != null && next.getPiece().getColor() == enemyColor && next.getPiece() instanceof King)
            return true; // Danger: piece could be captured by enemy king
        while (next != null) {
            if (next.getPiece() != null && next.getPiece().getColor() == enemyColor &&
                    (next.getPiece() instanceof Bishop || next.getPiece() instanceof Queen)) {
                return true; // Danger: piece could be captured by Bishops and Queens
            }
            else {
                if (next.getPiece() != null)
                    break;
                next = next.getNextBox(Box.Direction.LU);
            }
        }
        // Check danger in RD direction
        next = nextBoxes.get(Box.Direction.RD);
        if (next != null && next.getPiece() != null && next.getPiece().getColor() == enemyColor && next.getPiece() instanceof King)
            return true; // Danger: piece could be captured by enemy king
        while (next != null) {
            if (next.getPiece() != null && next.getPiece().getColor() == enemyColor &&
                    (next.getPiece() instanceof Bishop || next.getPiece() instanceof Queen)) {
                return true; // Danger: piece could be captured by Bishops and Queens
            }
            else {
                if (next.getPiece() != null)
                    break;
                next = next.getNextBox(Box.Direction.RD);
            }
        }
        // Check danger in RU direction
        next = nextBoxes.get(Box.Direction.RU);
        if (next != null && next.getPiece() != null && next.getPiece().getColor() == enemyColor && next.getPiece() instanceof King)
            return true; // Danger: piece could be captured by enemy king
        while (next != null) {
            if (next.getPiece() != null && next.getPiece().getColor() == enemyColor &&
                    (next.getPiece() instanceof Bishop || next.getPiece() instanceof Queen)) {
                return true; // Danger: piece could be captured by Bishops and Queens
            }
            else {
                if (next.getPiece() != null)
                    break;
                next = next.getNextBox(Box.Direction.RU);
            }
        }

        // Check enemy pawns in front
        if (enemyColor == Color.WHITE) {
            next = nextBoxes.get(Box.Direction.LD);
            if (next != null && next.getPiece() != null && next.getPiece() instanceof Pawn && next.getPiece().getColor() == enemyColor)
                return true;
            next = nextBoxes.get(Box.Direction.RD);
            if (next != null && next.getPiece() != null && next.getPiece() instanceof Pawn &&  next.getPiece().getColor() == enemyColor)
                return true;
        }
        else {
            next = nextBoxes.get(Box.Direction.LU);
            if (next != null && next.getPiece() != null && next.getPiece() instanceof Pawn && next.getPiece().getColor() == enemyColor)
                return true;
            next = nextBoxes.get(Box.Direction.RU);
            if (next != null && next.getPiece() != null && next.getPiece() instanceof Pawn && next.getPiece().getColor() == enemyColor)
                return true;
        }

        // Check horses
        next = getNextBox(Box.Direction.U);
        if (next != null) {
            next = next.getNextBox(Box.Direction.U);
            if (next != null) {
                // Test L and R
                Box left = next.getNextBox(Box.Direction.L);
                if (left != null && left.getPiece() != null && left.getPiece() instanceof  Horse && left.getPiece().getColor() == enemyColor)
                    return true;
                Box right = next.getNextBox(Box.Direction.R);
                if (right != null && right.getPiece() != null && right.getPiece() instanceof Horse && right.getPiece().getColor() == enemyColor)
                    return true;
            }
        }

        next = getNextBox(Box.Direction.L);
        if (next != null) {
            next = next.getNextBox(Box.Direction.L);
            if (next != null) {
                // Test U and D
                Box up = next.getNextBox(Box.Direction.U);
                if (up != null && up.getPiece() != null && up.getPiece() instanceof  Horse && up.getPiece().getColor() == enemyColor)
                    return true;
                Box down = next.getNextBox(Box.Direction.D);
                if (down != null && down.getPiece() != null && down.getPiece() instanceof  Horse && down.getPiece().getColor() == enemyColor)
                    return true;
            }
        }

        next = getNextBox(Box.Direction.D);
        if (next != null) {
            next = next.getNextBox(Box.Direction.D);
            if (next != null) {
                // Test L and R
                Box left = next.getNextBox(Box.Direction.L);
                if (left != null && left.getPiece() != null && left.getPiece() instanceof  Horse && left.getPiece().getColor() == enemyColor)
                    return true;
                Box right = next.getNextBox(Box.Direction.R);
                if (right != null && right.getPiece() != null && right.getPiece() instanceof Horse && right.getPiece().getColor() == enemyColor)
                    return true;
            }
        }

        next = getNextBox(Box.Direction.R);
        if (next != null) {
            next = next.getNextBox(Box.Direction.R);
            if (next != null) {
                // Test U and D
                Box up = next.getNextBox(Box.Direction.U);
                if (up != null && up.getPiece() != null && up.getPiece() instanceof  Horse && up.getPiece().getColor() == enemyColor)
                    return true;
                Box down = next.getNextBox(Box.Direction.D);
                if (down != null && down.getPiece() != null && down.getPiece() instanceof  Horse && down.getPiece().getColor() == enemyColor)
                    return true;
            }
        }

        return false;
    }
}
