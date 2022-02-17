package model;

import gui.NewGame;

import java.util.ArrayList;

/**
 * Game is class that contains all objects that make up a game of chess, namely board and history of moves.
 * Because the object wasn't to create program for playing chess, but to replay games (and add own moves), it does not contain player objects.
 *
 * @author  Natália Holková (xholko02)
 */
public class Game {
    private Board board;
    private Color turnColor;
    private ArrayList<Move> moveHistory; // First element in arrayList is first move, last is last move
    private ArrayList<Move> undoneMoveHistory; // First move is first undone (oldest), last in last undone (newest)
    private NewGame view;

    /**
     * Initialize Game object. Creates new {@link  Board Board} object and empty list of {@link Move Move} objects.
     */
    public Game() {
        System.out.println("Creating Game object...");

        board = new Board(); // Create new playing board
        moveHistory = new ArrayList<Move>(); // Create empty list of moves
        undoneMoveHistory = new ArrayList<Move>(); // Create empty list of undone moves
        turnColor = Color.WHITE; // White moves first
    }


    /**
     * Changes turn color to opposite.
     */
    public void updateTurnColor() {
        turnColor = (turnColor == Color.WHITE) ? Color.BLACK : Color.WHITE;
    }

    /**
     * Returns the board
     * @return Board
     */
    public Board getBoard() {
        return board;
    }

    /**
     * Returns board as 2D string
     * @return String
     */
    public String[][] boardToString(){
        Board b = this.board;
        String[][] strBoard = new String[b.SIZE][b.SIZE];

        // Get 2D string array representation of board for printing
        for (int row = 0; row < b.SIZE; row++) {
            for (int col = 0; col < b.SIZE; col++) {
                Box box = b.getBox(row, col);
                strBoard[row][col] = (box.getBoxColor() == Color.WHITE) ? "." : "x";
            }
        }

        ArrayList<Piece> whitePieces = b.getPieceSet(Color.WHITE).getPieces();
        for (Piece piece : whitePieces) {
            if (piece instanceof Pawn) {
                Box box = ((Pawn) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "p";
            }
            else if (piece instanceof King) {
                Box box = ((King) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "k";
            }
            else if (piece instanceof Queen) {
                Box box = ((Queen) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "d";//q
            }
            else if (piece instanceof Bishop) {
                Box box = ((Bishop) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "s";//b
            }
            else if (piece instanceof Horse) {
                Box box = ((Horse) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "j";//h
            }
            else if (piece instanceof Rook) {
                Box box = ((Rook) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "v";//r
            }
        }

        ArrayList<Piece> blackPieces = b.getPieceSet(Color.BLACK).getPieces();
        for (Piece piece : blackPieces) {
            if (piece instanceof Pawn) {
                Box box = ((Pawn) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "P";
            }
            else if (piece instanceof King) {
                Box box = ((King) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "K";
            }
            else if (piece instanceof Queen) {
                Box box = ((Queen) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "D";//Q
            }
            else if (piece instanceof Bishop) {
                Box box = ((Bishop) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "S";//B
            }
            else if (piece instanceof Horse) {
                Box box = ((Horse) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "J";//H
            }
            else if (piece instanceof Rook) {
                Box box = ((Rook) piece).getPosition();
                if (box != null) strBoard[box.getRow()][box.getCol()] = "V";//R
            }
        }
        return strBoard;
    }

    /**
     * Prints board
     */
    public void printBoard() {

        int size = this.board.SIZE;
        String[][] strBoard = boardToString();
        // Print board
        System.out.println("  a b c d e f g h");
        for (int row = size-1; row >= 0; row--) {
            System.out.print((row + 1) + "|");
            for (int col = 0; col < size; col++) {
                System.out.print(strBoard[row][col] + "|");
            }
            System.out.println();
        }
    }

    public Color getTurnColor() {
        return turnColor;
    }

    public boolean checkKingCheck(Color kingColor) {
        // Get king of correct color
        King king = null;
        PieceSet pieceSet = (kingColor == Color.WHITE) ? board.getPieceSet(Color.WHITE) : board.getPieceSet(Color.BLACK);
        for (Piece p : pieceSet.getPieces()) {
            if (p instanceof King) {
                king = (King) p;
                break;
            }
        }

        return king.isCheck();
    }

    public boolean checkKingCheckmate(Color kingColor) {
        // Get king of correct color
        King king = null;
        PieceSet pieceSet = (kingColor == Color.WHITE) ? board.getPieceSet(Color.WHITE) : board.getPieceSet(Color.BLACK);
        for (Piece p : pieceSet.getPieces()) {
            if (p instanceof King) {
                king = (King) p;
                break;
            }
        }

        return king.isCheckmate();
    }

    public boolean checkKingStalemate(Color kingColor) {
        // Get king of correct color
        boolean kingStalemate = false;
        boolean otherFiguresCanMove = false;
        PieceSet pieceSet = (kingColor == Color.WHITE) ? board.getPieceSet(Color.WHITE) : board.getPieceSet(Color.BLACK);
        for (Piece p : pieceSet.getPieces()) {
            if (p instanceof King) {
                kingStalemate = ((King) p).isStalemate();
            }
            else if (p instanceof Queen) {
                if (((Queen) p).getValidMoveBoxes().size() != 0) {
                    otherFiguresCanMove = true;
                    break;
                }
            }
            else if (p instanceof Rook) {
                if (((Rook) p).getValidMoveBoxes().size() != 0) {
                    otherFiguresCanMove = true;
                    break;
                }
            }
            else if (p instanceof Horse) {
                if (((Horse) p).getValidMoveBoxes().size() != 0) {
                    otherFiguresCanMove = true;
                    break;
                }
            }
            else if (p instanceof Bishop) {
                if (((Bishop) p).getValidMoveBoxes().size() != 0) {
                    otherFiguresCanMove = true;
                    break;
                }
            }
            else if (p instanceof Pawn) {
                if (((Pawn) p).getValidMoveBoxes().size() != 0) {
                    otherFiguresCanMove = true;
                    break;
                }
            }
        }

        return (kingStalemate && !otherFiguresCanMove);
    }

    // Call this BEFORE making move with figure
    public void appendMoveHistory(Move move) {
        moveHistory.add(move);
    }

    public Move getLastMove() {
        if (moveHistory.size() > 0)
            return moveHistory.get(moveHistory.size() - 1);
        else
            return null;
    }

    public ArrayList<Move> getHistory() {
        return moveHistory;
    }

    public void appendUndoneMoveHistory(Move undoneMove) {
        undoneMoveHistory.add(undoneMove);
    }

    public Move getLastUndoneMove() {
        if (undoneMoveHistory.size() > 0)
            return undoneMoveHistory.get(undoneMoveHistory.size() - 1);
        else
            return null;
    }

    public void clearUndoMoveHistory() {
        undoneMoveHistory.clear();
    }

    public boolean undo() {
        if (moveHistory.size() == 0)
            return false;

        // Get last move
        Move prevMove = moveHistory.get(moveHistory.size() - 1);

        // Remove piece from end box
        prevMove.getEndBox().setPiece(null);
        // Return moved piece back to start box
        prevMove.getStartBox().setPiece(prevMove.getMovedPiece());
        prevMove.getMovedPiece().setPosition(prevMove.getStartBox());

        // Return captured piece on board
        if (prevMove.getCapturedPiece() != null) {
            Piece captured = prevMove.getCapturedPiece();
            PieceSet pieceSet = getBoard().getPieceSet(captured.getColor());

            // remove from removed pieces
            pieceSet.getRemovedPieces().remove(captured);

            // add to list of piece
            pieceSet.getPieces().add(captured);

            // set on box
            captured.setPosition(prevMove.getEndBox());
            prevMove.getEndBox().setPiece(captured);
        }

        // Remove last element in move history
        moveHistory.remove(moveHistory.size()-1);

        appendUndoneMoveHistory(prevMove);

        return true;
    }

    public boolean redo() {
        if (undoneMoveHistory.size() == 0)
            return false;

        // Get last undone move
        Move undoneMove = undoneMoveHistory.get(undoneMoveHistory.size() - 1);

        // Make the move
        undoneMove.getMovedPiece().moveToPosition(undoneMove.getEndBox());

        // Remove last element in undone move history
        undoneMoveHistory.remove(undoneMoveHistory.size()-1);

        appendMoveHistory(undoneMove);

        return true;
    }

    public ArrayList<Move> getUndoneHistory() {
        return undoneMoveHistory;
    }
}
