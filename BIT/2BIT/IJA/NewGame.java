package gui;

import javafx.geometry.Insets;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.*;
import javafx.scene.paint.ImagePattern;
import javafx.scene.shape.Circle;
import model.*;
import javafx.event.EventHandler;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.FontPosture;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.stage.Stage;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import model.*;

public class NewGame {

    Tab tab;
    double orgSceneX, orgSceneY, orgTranslateX, orgTranslateY;
    Game game;
    ArrayList<Circle> validBoxesCircles;
    Group groupChessBoard;
    Group groupRemovedPieces;
    Box selectedBox;
    ImageView[] whiteFigures, blackFigures;
    int whiteRemoved;
    int blackRemoved;
    HashMap<Piece, ImageView> removedMap;
    HashMap<Piece, ImageView> onBoardMap;
    Label onTurnLabel;
    ListView movesListView;
    int moveCount, prevSelected;
    Button undoButton, redoButton;
    Spinner<Integer> spinner;

    public enum SpecialState {
        NONE, CHECK, CHECKMATE, STALEMATE
    }
    SpecialState whiteState, blackState;

    /**
     *
     * @param tabCounter
     */
    public NewGame(int tabCounter){

        // CREATE GAME
        game = GameFactory.createChessGame();
        moveCount = 0;

        tab = new Tab("Game_" + (tabCounter+1));
        // Chessboard
        validBoxesCircles = new ArrayList<Circle>();
        groupChessBoard = new Group();
        groupRemovedPieces = new Group();
        whiteRemoved = 0;
        blackRemoved = 0;
        Rectangle[][] chessBoxes = new Rectangle[10][10];
        for (int row = 9; row >= 0; row--) {
            for (int col = 0; col < 10; col++) {
                chessBoxes[row][col] = new Rectangle();
                chessBoxes[row][col].setWidth(40);
                chessBoxes[row][col].setHeight(40);
                chessBoxes[row][col].setX(40*(9 - row));
                chessBoxes[row][col].setY(40*col);

                // Choose fill color
                if (row == 9 || row == 0 || col == 9 || col == 0)
                    chessBoxes[row][col].setFill(Color.CORNSILK);
                else if ((row % 2 == 1 && col % 2 == 1) || (row % 2 == 0 && col % 2 == 0))
                    chessBoxes[row][col].setFill(Color.SIENNA);
                else
                    chessBoxes[row][col].setFill(Color.WHEAT);

                groupChessBoard.getChildren().add(chessBoxes[row][col]);
            }
        }
        createBoxesAroundChessBoard(groupChessBoard);

        // Right side
        // ListView of moves
        VBox rightSide = new VBox();
        rightSide.setAlignment(Pos.TOP_RIGHT);
        rightSide.setSpacing(10);
//        tab.setContent(rightSide);
        onTurnLabel = new Label();
        onTurnLabel.setText("On turn: " + ((game.getTurnColor() == model.Color.WHITE) ? "WHITE" : "BLACK"));
//        tab.setContent(onTurnLabel);
//        tab.setContent(moveLabel);
        movesListView = new ListView<>();

        //movesListView.getItems().addAll("Move 1", "Move 2", "Move 3");
        movesListView.getSelectionModel().setSelectionMode(SelectionMode.SINGLE);
        movesListView.setMaxHeight(400);
        movesListView.setMaxWidth(200);

        // Set listView item events
        movesListView.setOnMouseClicked(movesListClicked);
        prevSelected = 0;

//        Label onTurnLabel = new Label();
//        //todo aby sa obnovovalo po kazdom tahu
//        onTurnLabel.setText("On turn:" + turnNo);
//        Label moveLabel = new Label("Moves");
//
//        if (moves != null) {
//            ObservableList<String> items = FXCollections.observableArrayList(moves);
//            movesListView.setItems(items);*/
//            /*
//        movesListView.getSelectionModel().selectedItemProperty().addListener((ObservableValue<? extends String> ov, String old_val, String new_val) -> {
//            //String selectedItem = movesListView.getSelectionModel().getSelectedItem();
//            int index = movesListView.getSelectionModel().getSelectedIndex();
//           // System.out.println(movesListView.getSelectionModel().getSelectedIndex());
//            makeMoves(moves, index);
//        });*/}
//
//        ListView movesListView = new ListView<>();
//        if (moves != null) {
//        movesListView.getItems().addAll(moves); }

        rightSide.getChildren().addAll(onTurnLabel, movesListView);

        // Bottom side
        ToggleGroup modeGroup = new ToggleGroup();
        RadioButton manualButton = new RadioButton("Manual");
        manualButton.setToggleGroup(modeGroup);
        manualButton.setSelected(true);
        RadioButton autoButton = new RadioButton("Automatic");
        autoButton.setToggleGroup(modeGroup);

        manualButton.setOnMouseClicked(manualButtonClicked);
        autoButton.setOnMouseClicked(autoButtonClicked);

        Button prevStepButton = new Button("Previous Step");
        Button nextStepButton = new Button("Next Step");
        undoButton = new Button("Undo");
        redoButton = new Button("redo");

        // Speed spinner
        Label speedlabel = new Label("Select speed:");
        spinner = new Spinner<Integer>();
        final int initialValue = 1;
        // Value factory.
        SpinnerValueFactory<Integer> valueFactory = new SpinnerValueFactory.IntegerSpinnerValueFactory(1, 10, initialValue);
        spinner.setValueFactory(valueFactory);

        GridPane gridPaneBottom = new GridPane();
        gridPaneBottom.setPadding(new Insets(10, 10, 10, 10)); //Setting the padding
        gridPaneBottom.setVgap(5); //Setting the vertical gap between the columns
        gridPaneBottom.setHgap(5); //Setting the horizontal gap between the columns
        gridPaneBottom.setAlignment(Pos.CENTER); //Setting the Grid alignment
        //Arranging all the nodes in the grid
        gridPaneBottom.add(manualButton, 0, 0);
        gridPaneBottom.add(autoButton, 1, 0);
        gridPaneBottom.add(speedlabel, 0, 1);
        gridPaneBottom.add(spinner, 1, 1);
        gridPaneBottom.add(prevStepButton, 2, 0);
        gridPaneBottom.add(nextStepButton, 2, 1);
        gridPaneBottom.add(undoButton, 3, 0);
        gridPaneBottom.add(redoButton, 3, 1);

        // Start new game
        removedMap = new HashMap<Piece, ImageView>();
        onBoardMap = new HashMap<Piece, ImageView>();

        game.printBoard();
        // create rectangles with figure images
        // White pieces
        whiteFigures = setDefaultWhiteFigures(groupChessBoard);
        // Black pieces
        blackFigures = setDefaultBlackFigures(groupChessBoard);

        // Set figure events
        setFiguresEvents(whiteFigures, blackFigures);

        // Set button events
        undoButton.setOnMouseClicked(undoButtonClicked);
        redoButton.setOnMouseClicked(redoButtonClicked);
        prevStepButton.setOnMouseClicked(prevButtonClicked);
        nextStepButton.setOnMouseClicked(nextButtonClicked);

        /*
        // Top menu
        Menu gameMenu = new Menu("Game");
        // Menu items
        gameMenu.getItems().add(new MenuItem("Save Game"));
        gameMenu.getItems().add(new MenuItem("Load Game"));
        // Main menu bar
        MenuBar menuBar = new MenuBar();
        menuBar.getMenus().addAll(gameMenu);
        */

        // Set layout
        BorderPane layout = new BorderPane();
        //layout.setTop(menuBar);
        layout.setCenter(groupChessBoard);
        layout.setRight(rightSide);
        layout.setLeft(groupRemovedPieces);
        layout.setBottom(gridPaneBottom);

        FlowPane pane = new FlowPane();
        pane.setPadding(new Insets(10, 10, 10,10));
        //pane.setVgap(20);
        pane.setHgap(160);
      //  pane.setPrefWrapLength(210);
        pane.getChildren().add(layout);
        tab.setContent(pane);
    }

    /**
     *
     * @param window
     */
    private static void closeProgram(Stage window) {
        window.close();
    }

    /**
     *
     * @param groupChessBoard
     */
    public void createBoxesAroundChessBoard(Group groupChessBoard) {
        // Top chessboard text
        Text[] topText = new Text[8];
        for (int i = 0; i < 8; i++) {
            topText[i] = new Text();
            topText[i].setFont(Font.font("verdana", FontWeight.BOLD, FontPosture.REGULAR, 20));
            topText[i].setText(Character.toString((char) ('A' + i)));
            topText[i].setX(40 + 10 + i * 40);
            topText[i].setY(30);

            groupChessBoard.getChildren().add(topText[i]);
        }
        // Bottom chessboard text
        Text[] bottomText = new Text[8];
        for (int i = 0; i < 8; i++) {
            bottomText[i] = new Text();
            bottomText[i].setFont(Font.font("verdana", FontWeight.BOLD, FontPosture.REGULAR, 20));
            bottomText[i].setText(Character.toString((char) ('A' + i)));
            bottomText[i].setX(40 + 10 + i * 40);
            bottomText[i].setY(9*40 + 30);

            groupChessBoard.getChildren().add(bottomText[i]);
        }
        // Left chessboard text
        Text[] leftText = new Text[8];
        for (int i = 0; i < 8; i++) {
            leftText[i] = new Text();
            leftText[i].setFont(Font.font("verdana", FontWeight.BOLD, FontPosture.REGULAR, 20));
            leftText[i].setText(Character.toString((char) ('1' + i)));
            leftText[i].setX(10);
            leftText[i].setY(9*40 - 10 - i*40);

            groupChessBoard.getChildren().add(leftText[i]);
        }
        // Left chessboard text
        Text[] rightText = new Text[8];
        for (int i = 0; i < 8; i++) {
            rightText[i] = new Text();
            rightText[i].setFont(Font.font("verdana", FontWeight.BOLD, FontPosture.REGULAR, 20));
            rightText[i].setText(Character.toString((char) ('1' + i)));
            rightText[i].setX(9*40 + 10);
            rightText[i].setY(9*40 - 10 - i*40);

            groupChessBoard.getChildren().add(rightText[i]);
        }
    }

    /**
     *
     * @param groupChessBoard
     * @return
     */
    private ImageView[] setDefaultWhiteFigures(Group groupChessBoard) {
        ImageView[] whiteFigures = new ImageView[16];

        // White king
        ImageView whiteKing = new ImageView(new Image(NewGame.class.getClassLoader().getResource("whiteKing.png").toString()));
        //ImageView whiteKing = new ImageView(new Image(NewGame.class.getResource("/lib/images/whiteKing.png").toString()));
        whiteKing.setFitHeight(40);
        whiteKing.setFitWidth(40);
        whiteKing.setX(5*40);
        whiteKing.setY(8*40);
        groupChessBoard.getChildren().add(whiteKing);
        // White queen
        ImageView whiteQueen = new ImageView(new Image(NewGame.class.getClassLoader().getResource("whiteQueen.png").toString()));
        whiteQueen.setFitHeight(40);
        whiteQueen.setFitWidth(40);
        whiteQueen.setX(4*40);
        whiteQueen.setY(8*40);
        groupChessBoard.getChildren().add(whiteQueen);
        // White rook 1
        ImageView whiteRook1 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("whiteRook.png").toString()));
        whiteRook1.setFitHeight(40);
        whiteRook1.setFitWidth(40);
        whiteRook1.setX(1*40);
        whiteRook1.setY(8*40);
        groupChessBoard.getChildren().add(whiteRook1);
        // White rook 2
        ImageView whiteRook2 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("whiteRook.png").toString()));
        whiteRook2.setFitHeight(40);
        whiteRook2.setFitWidth(40);
        whiteRook2.setX(8*40);
        whiteRook2.setY(8*40);
        groupChessBoard.getChildren().add(whiteRook2);
        // White horse 1
        ImageView whiteHorse1 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("whiteHorse.png").toString()));
        whiteHorse1.setFitHeight(40);
        whiteHorse1.setFitWidth(40);
        whiteHorse1.setX(2*40);
        whiteHorse1.setY(8*40);
        groupChessBoard.getChildren().add(whiteHorse1);
        // White horse 2
        ImageView whiteHorse2 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("whiteHorse.png").toString()));
        whiteHorse2.setFitHeight(40);
        whiteHorse2.setFitWidth(40);
        whiteHorse2.setX(7*40);
        whiteHorse2.setY(8*40);
        groupChessBoard.getChildren().add(whiteHorse2);
        // White bishop 1
        ImageView whiteBishop1 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("whiteBishop.png").toString()));
        whiteBishop1.setFitHeight(40);
        whiteBishop1.setFitWidth(40);
        whiteBishop1.setX(3*40);
        whiteBishop1.setY(8*40);
        groupChessBoard.getChildren().add(whiteBishop1);
        // White bishop 2
        ImageView whiteBishop2 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("whiteBishop.png").toString()));
        whiteBishop2.setFitHeight(40);
        whiteBishop2.setFitWidth(40);
        whiteBishop2.setX(6*40);
        whiteBishop2.setY(8*40);
        groupChessBoard.getChildren().add(whiteBishop2);

        whiteFigures[0] = whiteKing;
        whiteFigures[1] = whiteQueen;
        whiteFigures[2] = whiteRook1;
        whiteFigures[3] = whiteRook2;
        whiteFigures[4] = whiteHorse1;
        whiteFigures[5] = whiteHorse2;
        whiteFigures[6] = whiteBishop1;
        whiteFigures[7] = whiteBishop2;

        for (int i = 0; i < 8; i++) {
            ImageView whitePawn = new ImageView(new Image(NewGame.class.getClassLoader().getResource("whitePawn.png").toString()));
            whitePawn.setFitHeight(40);
            whitePawn.setFitWidth(40);
            whitePawn.setX(40 + i*40);
            whitePawn.setY(7*40);
            groupChessBoard.getChildren().add(whitePawn);

            whiteFigures[8 + i] = whitePawn;
        }

        // Bind to hash map
        int bishops = 0, horses = 0, rooks = 0, pawns = 0;
        for (Piece p : game.getBoard().getPieceSet(model.Color.WHITE).getPieces()) {
            if (p instanceof King)
                onBoardMap.put(p, whiteKing);
            else if (p instanceof  Queen)
                onBoardMap.put((Queen) p, whiteQueen);
            else if (p instanceof  Bishop)
                onBoardMap.put((Bishop) p, (bishops++ == 0) ? whiteBishop1 : whiteBishop2);
            else if (p instanceof  Horse)
                onBoardMap.put((Horse) p, (horses++ == 0) ? whiteHorse1 : whiteHorse2);
            else if (p instanceof  Rook)
                onBoardMap.put((Rook) p, (rooks++ == 0) ? whiteRook1 : whiteRook2);
            else if (p instanceof Pawn)
                onBoardMap.put((Pawn) p, whiteFigures[8 + pawns++]);
        }

        return whiteFigures;
    }

    /**
     *
     * @param groupChessBoard
     * @return
     */
    private ImageView[] setDefaultBlackFigures(Group groupChessBoard) {
        ImageView[] blackFigures = new ImageView[16];
        // Black king
        ImageView blackKing = new ImageView(new Image(NewGame.class.getClassLoader().getResource("blackKing.png").toString()));
        blackKing.setFitHeight(40);
        blackKing.setFitWidth(40);
        blackKing.setX(5*40);
        blackKing.setY(1*40);
        groupChessBoard.getChildren().add(blackKing);
        // Black queen
        ImageView blackQueen = new ImageView(new Image(NewGame.class.getClassLoader().getResource("blackQueen.png").toString()));
        blackQueen.setFitHeight(40);
        blackQueen.setFitWidth(40);
        blackQueen.setX(4*40);
        blackQueen.setY(1*40);
        groupChessBoard.getChildren().add(blackQueen);
        // Black rook 1
        ImageView blackRook1 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("blackRook.png").toString()));
        blackRook1.setFitHeight(40);
        blackRook1.setFitWidth(40);
        blackRook1.setX(1*40);
        blackRook1.setY(1*40);
        groupChessBoard.getChildren().add(blackRook1);
        // Black rook 2
        ImageView blackRook2 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("blackRook.png").toString()));
        blackRook2.setFitHeight(40);
        blackRook2.setFitWidth(40);
        blackRook2.setX(8*40);
        blackRook2.setY(1*40);
        groupChessBoard.getChildren().add(blackRook2);
        // Black horse 1
        ImageView blackHorse1 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("blackHorse.png").toString()));
        blackHorse1.setFitHeight(40);
        blackHorse1.setFitWidth(40);
        blackHorse1.setX(2*40);
        blackHorse1.setY(1*40);
        groupChessBoard.getChildren().add(blackHorse1);
        // Black horse 2
        ImageView blackHorse2 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("blackHorse.png").toString()));
        blackHorse2.setFitHeight(40);
        blackHorse2.setFitWidth(40);
        blackHorse2.setX(7*40);
        blackHorse2.setY(1*40);
        groupChessBoard.getChildren().add(blackHorse2);
        // Black bishop 1
        ImageView blackBishop1 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("blackBishop.png").toString()));
        blackBishop1.setFitHeight(40);
        blackBishop1.setFitWidth(40);
        blackBishop1.setX(3*40);
        blackBishop1.setY(1*40);
        groupChessBoard.getChildren().add(blackBishop1);
        // Black bishop 2
        ImageView blackBishop2 = new ImageView(new Image(NewGame.class.getClassLoader().getResource("blackBishop.png").toString()));
        blackBishop2.setFitHeight(40);
        blackBishop2.setFitWidth(40);
        blackBishop2.setX(6*40);
        blackBishop2.setY(1*40);
        groupChessBoard.getChildren().add(blackBishop2);

        blackFigures[0] = blackKing;
        blackFigures[1] = blackQueen;
        blackFigures[2] = blackRook1;
        blackFigures[3] = blackRook2;
        blackFigures[4] = blackHorse1;
        blackFigures[5] = blackHorse2;
        blackFigures[6] = blackBishop1;
        blackFigures[7] = blackBishop2;

        for (int i = 0; i < 8; i++) {
            ImageView blackPawn = new ImageView(new Image(NewGame.class.getClassLoader().getResource("blackPawn.png").toString()));
            blackPawn.setFitHeight(40);
            blackPawn.setFitWidth(40);
            blackPawn.setX(40 + i*40);
            blackPawn.setY(2*40);
            groupChessBoard.getChildren().add(blackPawn);

            blackFigures[8 + i] = blackPawn;
        }

        // Bind to hash map
        int bishops = 0, horses = 0, rooks = 0, pawns = 0;
        for (Piece p : game.getBoard().getPieceSet(model.Color.BLACK).getPieces()) {
            if (p instanceof King)
                onBoardMap.put((King) p, blackKing);
            else if (p instanceof  Queen)
                onBoardMap.put((Queen) p, blackQueen);
            else if (p instanceof  Bishop)
                onBoardMap.put((Bishop) p, (bishops++ == 0) ? blackBishop1 : blackBishop2);
            else if (p instanceof  Horse)
                onBoardMap.put((Horse) p, (horses++ == 0) ? blackHorse1 : blackHorse2);
            else if (p instanceof  Rook)
                onBoardMap.put((Rook) p, (rooks++ == 0) ? blackRook1 : blackRook2);
            else if (p instanceof Pawn)
                onBoardMap.put((Pawn) p, blackFigures[8 + pawns++]);
        }

        return blackFigures;
    }

    EventHandler<MouseEvent> manualButtonClicked = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            System.out.println("MANUAL mode");

            // Set undo, redo buttons as clickable
            undoButton.setDisable(false);
            redoButton.setDisable(false);
            spinner.setDisable(true);
        }
    };

    EventHandler<MouseEvent> autoButtonClicked = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            System.out.println("AUTO mode");

            // Set undo, redo buttons as unclickable
            undoButton.setDisable(true);
            redoButton.setDisable(true);
            spinner.setDisable(false);
        }
    };

    /**
     *
     * @param whiteFigures
     * @param blackFigures
     */
    private void setFiguresEvents(ImageView[] whiteFigures, ImageView[] blackFigures) {
        for (int i = 0; i < 16; i++) {

            whiteFigures[i].setOnMousePressed(figureOnMousePressedEventHandler);
            blackFigures[i].setOnMousePressed(figureOnMousePressedEventHandler);

            whiteFigures[i].setOnMouseDragged(figureOnMouseDraggedEventHandler);
            blackFigures[i].setOnMouseDragged(figureOnMouseDraggedEventHandler);

            whiteFigures[i].setOnMouseReleased(figureOnMouseReleasedEventHandler);
            blackFigures[i].setOnMouseReleased(figureOnMouseReleasedEventHandler);
        }
    }

    EventHandler<MouseEvent> movesListClicked = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            System.out.println("You clicked on list item!");
            int selIndex = (movesListView.getSelectionModel().getSelectedIndex() + 1) * 2 ;
            System.out.println("Selected index: " + selIndex);
            System.out.println("history size: " + game.getHistory().size());

            if (selIndex < game.getHistory().size() - 1) {
                System.out.println("Going backwards");
                int currIndex = game.getHistory().size();
                // Going backward - undo
                while (currIndex > selIndex) {
                    undo();
                    currIndex--;
                }
            }
            else if (selIndex > game.getHistory().size() - 1) {
                // Going forwand
                System.out.println("Going forward");
                int currIndex = game.getHistory().size();
                System.out.println("Curr: " + currIndex);
                // Going backward - undo
                while (currIndex < selIndex) {
                    // Get notation
                    System.out.println("curr: " + currIndex);
                    String notation = movesListView.getItems().get(currIndex / 2).toString();
                    System.out.println(currIndex + " " + notation);

                    // TODO
                    // z notation ziskať Move whiteMove a Move blackMove
                    // vykonať tie pohyby v gui a v modeli

                    // Get last from undoneMoveHistory
                    Move lastUndoneMove = game.getLastUndoneMove();
                    if (lastUndoneMove != null) {
                        Piece movedPiece = lastUndoneMove.getMovedPiece();
                        Piece capturedPiece = lastUndoneMove.getCapturedPiece();
                        Box endBox = lastUndoneMove.getEndBox();
                        Box startBox = lastUndoneMove.getStartBox();
                        // Get correct ImageView
                        ImageView movedPieceImg = onBoardMap.get(movedPiece);

                        // Move the image
                        setDownFigure(movedPieceImg, endBox);

                        // If pawn, rook or king - decrease wasMoved
                        if (movedPiece instanceof Pawn)
                            ((Pawn) movedPiece).incWasMoved();
                        else if (movedPiece instanceof Rook)
                            ((Rook) movedPiece).incWasMoved();
                        else if (movedPiece instanceof  King)
                            ((King) movedPiece).incWasMoved();

                        // If captured piece - put him to side
                        if (capturedPiece != null) {
                            removeImg(capturedPiece, endBox);
                        }

                        // Make change in the model
                        game.redo();

                        checkState();

                        // Change turn
                        game.updateTurnColor();
                        onTurnLabel.setText("On turn: " + ((game.getTurnColor() == model.Color.WHITE) ? "WHITE" : "BLACK"));

                        // Increase move count
                        moveCount++;
                    }

                    lastUndoneMove = game.getLastUndoneMove();
                    if (lastUndoneMove != null) {
                        Piece movedPiece = lastUndoneMove.getMovedPiece();
                        Piece capturedPiece = lastUndoneMove.getCapturedPiece();
                        Box endBox = lastUndoneMove.getEndBox();
                        Box startBox = lastUndoneMove.getStartBox();
                        // Get correct ImageView
                        ImageView movedPieceImg = onBoardMap.get(movedPiece);

                        // Move the image
                        setDownFigure(movedPieceImg, endBox);

                        // If pawn, rook or king - decrease wasMoved
                        if (movedPiece instanceof Pawn)
                            ((Pawn) movedPiece).incWasMoved();
                        else if (movedPiece instanceof Rook)
                            ((Rook) movedPiece).incWasMoved();
                        else if (movedPiece instanceof  King)
                            ((King) movedPiece).incWasMoved();

                        // If captured piece - put him to side
                        if (capturedPiece != null) {
                            removeImg(capturedPiece, endBox);
                        }

                        // Make change in the model
                        game.redo();

                        checkState();

                        // Change turn
                        game.updateTurnColor();
                        onTurnLabel.setText("On turn: " + ((game.getTurnColor() == model.Color.WHITE) ? "WHITE" : "BLACK"));

                        // Increase move count
                        moveCount++;

                    }

                    currIndex += 2;
                }
            }

        }
    };

    public void undo() {
        Move lastMove = game.getLastMove();
        if (lastMove != null) {
            Piece movedPiece = lastMove.getMovedPiece();
            Piece capturedPiece = lastMove.getCapturedPiece();
            Box endBox = lastMove.getEndBox();
            Box startBox = lastMove.getStartBox();
            // Get correct ImageView
            ImageView movedPieceImg = onBoardMap.get(movedPiece);

            // Move the image
            setDownFigure(movedPieceImg, startBox);

            // If pawn, rook or king - decrease wasMoved
            if (movedPiece instanceof Pawn)
                ((Pawn) movedPiece).descWasMoved();
            else if (movedPiece instanceof Rook)
                ((Rook) movedPiece).descWasMoved();
            else if (movedPiece instanceof  King)
                ((King) movedPiece).descWasMoved();

            // Return captured piece (if any)
            if (capturedPiece != null) {
                ImageView capturedPieceImg = null;
                // using for-each loop for iteration over Map.entrySet()
                for (HashMap.Entry<Piece, ImageView> entry : removedMap.entrySet()) {
                    Piece p = entry.getKey();
                    if (p == capturedPiece) {
                        capturedPieceImg = entry.getValue();
                        break;
                    }
                }
                capturedPieceImg.setX(40 + endBox.getCol() * 40);
                capturedPieceImg.setY(8 * 40 - endBox.getRow() * 40);

                groupRemovedPieces.getChildren().remove(capturedPieceImg);
                groupChessBoard.getChildren().add(capturedPieceImg);
            }

            // Make change in the model
            game.undo();

            checkState(); // TODO is it ok?

            // Change turn
            game.updateTurnColor();
            onTurnLabel.setText("On turn: " + ((game.getTurnColor() == model.Color.WHITE) ? "WHITE" : "BLACK"));

            // Add move to undoneMoveHistory
            //game.appendUndoneMoveHistory(lastMove);

            // Remove move from history
            //game.getHistory().remove(lastMove);

            // Decrease move count
            moveCount--;

            System.out.println("UNDO history " + game.getHistory().size() + ", undone hist: " + game.getUndoneHistory().size());
        }
    }

    EventHandler<MouseEvent> prevButtonClicked = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            if (game.getHistory().size() % 2 == 0) {
                undo();
                undo();
            }
            else {
                undo();
            }
            // Change highlight
            int row = game.getHistory().size() / 2 - 1;
            System.out.println("Row: " + row);
            if (row < 0) {
                // none highlighted

            }
            else {
                movesListView.requestFocus();
                movesListView.getSelectionModel().select(row);
                movesListView.getFocusModel().focus(row);
            }
        }
    };

    EventHandler<MouseEvent> nextButtonClicked = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            if (game.getHistory().size() % 2 == 0) {
                redo();
                redo();
            }
            else {
                redo();
            }
            // Change highlight
            int row = game.getHistory().size() / 2 - 1;
            System.out.println("Row: " + row);
            if (row < 0) {
                // none highlighted

            }
            else {
                movesListView.requestFocus();
                movesListView.getSelectionModel().select(row);
                movesListView.getFocusModel().focus(row);
            }
        }
    };

    EventHandler<MouseEvent> undoButtonClicked = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            undo();
            if (moveCount % 2 == 1) {
                // Remove last entry in listView
                int rowCount = movesListView.getItems().size();
                movesListView.getItems().remove(rowCount - 1);
            }
        }
    };

    public void redo() {
        Move lastUndoneMove = game.getLastUndoneMove();
        if (lastUndoneMove != null) {
            Piece movedPiece = lastUndoneMove.getMovedPiece();
            Piece capturedPiece = lastUndoneMove.getCapturedPiece();
            Box endBox = lastUndoneMove.getEndBox();
            Box startBox = lastUndoneMove.getStartBox();
            // Get correct ImageView
            ImageView movedPieceImg = onBoardMap.get(movedPiece);

            // Move the image
            setDownFigure(movedPieceImg, endBox);

            // If pawn, rook or king - decrease wasMoved
            if (movedPiece instanceof Pawn)
                ((Pawn) movedPiece).incWasMoved();
            else if (movedPiece instanceof Rook)
                ((Rook) movedPiece).incWasMoved();
            else if (movedPiece instanceof  King)
                ((King) movedPiece).incWasMoved();

            // If captured piece - put him to side
            if (capturedPiece != null) {
                removeImg(capturedPiece, endBox);
            }

            // Make change in the model
            game.redo();

            checkState();

            // Change turn
            game.updateTurnColor();
            onTurnLabel.setText("On turn: " + ((game.getTurnColor() == model.Color.WHITE) ? "WHITE" : "BLACK"));

            // Increase move count
            moveCount++;

            System.out.println("REDO history " + game.getHistory().size() + ", undone hist: " + game.getUndoneHistory().size());
            // If now on turn white - print out notation

        }
    }

    EventHandler<MouseEvent> redoButtonClicked = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            Move lastUndoneMove = game.getLastUndoneMove();
            if (lastUndoneMove != null) {
                Piece movedPiece = lastUndoneMove.getMovedPiece();
                Piece capturedPiece = lastUndoneMove.getCapturedPiece();
                Box endBox = lastUndoneMove.getEndBox();
                Box startBox = lastUndoneMove.getStartBox();
                // Get correct ImageView
                ImageView movedPieceImg = onBoardMap.get(movedPiece);

                // Move the image
                setDownFigure(movedPieceImg, endBox);

                // If pawn, rook or king - decrease wasMoved
                if (movedPiece instanceof Pawn)
                    ((Pawn) movedPiece).incWasMoved();
                else if (movedPiece instanceof Rook)
                    ((Rook) movedPiece).incWasMoved();
                else if (movedPiece instanceof  King)
                    ((King) movedPiece).incWasMoved();

                // If captured piece - put him to side
                if (capturedPiece != null) {
                    removeImg(capturedPiece, endBox);
                }

                // Make change in the model
                game.redo();

                checkState();

                // Change turn
                game.updateTurnColor();
                onTurnLabel.setText("On turn: " + ((game.getTurnColor() == model.Color.WHITE) ? "WHITE" : "BLACK"));

                // Increase move count
                moveCount++;

                System.out.println("REDO history " + game.getHistory().size() + ", undone hist: " + game.getUndoneHistory().size());
                // If now on turn white - print out notation

                if (game.getTurnColor() == model.Color.WHITE && game.getHistory().size() >= 2) {
                    Move whiteMove = game.getHistory().get(game.getHistory().size() - 2); // White move was second to last
                    Move blackMove = game.getLastMove(); // Black move was last
                    String turnNotation = parseMoveToString(whiteMove, blackMove, (moveCount + 1) / 2);
                    System.out.println(turnNotation);

                    movesListView.getItems().add(turnNotation);
                }

            }
        }
    };

    EventHandler<MouseEvent> figureOnMousePressedEventHandler = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            // Determine color of clicked figure
            model.Color clickedCol = model.Color.BLACK;
            for (ImageView img : whiteFigures) {
                if (img.equals((ImageView)(t.getSource()))) {
                    clickedCol = model.Color.WHITE;
                    break;
                }
            }
            System.out.format("This turn will be: %s\n", (game.getTurnColor() == model.Color.WHITE) ? "WHITE" : "BLACK");
            if (game.getTurnColor() == clickedCol) {

                System.out.println("Figure clicked");
                orgSceneX = t.getSceneX();
                orgSceneY = t.getSceneY();
                orgTranslateX = ((ImageView) (t.getSource())).getTranslateX();
                orgTranslateY = ((ImageView) (t.getSource())).getTranslateY();

                int imgX = (int) ((ImageView) (t.getSource())).getX();
                int imgY = (int) ((ImageView) (t.getSource())).getY();
                System.out.format("imgX: %d, imgY: %d\n", imgX, imgY);
                System.out.format("Origin: row %d, col %d\n", decodeRow(imgY), decodeCol(imgX));

                // Display possible moves
                Box box = game.getBoard().getBox(decodeRow(imgY), decodeCol(imgX));
                selectedBox = box;
                if (box.getPiece() != null) {
                    Piece piece = box.getPiece();
                    ArrayList<Box> moveBoxes = piece.getValidMoveBoxes();
                    for (Box moveBox : moveBoxes) {
                        int row = moveBox.getRow();
                        int col = moveBox.getCol();
                        System.out.format("Valid move: row %d, col %d\n", row, col);
                        int dot_y = 8 * 40 - row * 40;
                        int dot_x = 40 + col * 40;

                        Circle circle = new Circle(dot_x + 20, dot_y + 20, 10);
                        circle.setFill(Color.CHARTREUSE);
                        validBoxesCircles.add(circle);
                        groupChessBoard.getChildren().add(circle);

                    }
                }
            }
            else {
                System.out.println("Not on turn");
            }
        }
    };

    EventHandler<MouseEvent> figureOnMouseDraggedEventHandler = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            // Determine color of clicked figure
            model.Color clickedCol = determineClickedCol((ImageView)(t.getSource()));

            if (game.getTurnColor() == clickedCol) {

                double offsetX = t.getSceneX() - orgSceneX;
                double offsetY = t.getSceneY() - orgSceneY;
                double newTranslateX = orgTranslateX + offsetX;
                double newTranslateY = orgTranslateY + offsetY;

                ((ImageView) (t.getSource())).setTranslateX(newTranslateX);
                ((ImageView) (t.getSource())).setTranslateY(newTranslateY);
            }
        }
    };

    EventHandler<MouseEvent> figureOnMouseReleasedEventHandler = new EventHandler<MouseEvent>() {
        @Override
        public void handle(MouseEvent t) {
            // Determine color of clicked figure
            model.Color clickedCol = determineClickedCol(((ImageView) (t.getSource())));

            if (game.getTurnColor() == clickedCol) {

                double finalX = t.getSceneX();
                double finalY = t.getSceneY();
                double movedX = finalX - orgSceneX;
                double movedY = finalY - orgSceneY;

                int posOnBoardX = (int) (((ImageView) (t.getSource())).getX() + roundToNearest40((int) movedX));
                int posOnBoardY = (int) (((ImageView) (t.getSource())).getY() + roundToNearest40((int) movedY));

                System.out.format("posOnBoardX: %d, posOnBoardY: %d\n", posOnBoardX, posOnBoardY);
                System.out.format("New: row %d, col %d\n", decodeRow(posOnBoardY), decodeCol(posOnBoardX));

                // Remove possible moves
                for (Circle circle : validBoxesCircles) {
                    groupChessBoard.getChildren().remove(circle);
                }
                validBoxesCircles.removeAll(validBoxesCircles);

                Box newBox = game.getBoard().getBox(decodeRow(posOnBoardY), decodeCol(posOnBoardX));
                if (selectedBox.getPiece().getValidMoveBoxes().contains(newBox)) {


                    // Make change also in model
                    Move move = selectedBox.getPiece().moveToPosition(newBox);
                    game.appendMoveHistory(move); // Add move to history of moves
                    game.clearUndoMoveHistory();
                    if (move.getCapturedPiece() != null) {
                        Piece captured = move.getCapturedPiece();
                        removeImg(captured, newBox);
                    }

                    // Valid move - set down the figure
                    setDownFigure(((ImageView) (t.getSource())), posOnBoardX, posOnBoardY);

                    // Check promotion
                    promotePawn(newBox, ((ImageView) t.getSource()));

                    game.printBoard();

                    checkState();

                    // Update turn
                    game.updateTurnColor();
                    onTurnLabel.setText("On turn: " + ((game.getTurnColor() == model.Color.WHITE) ? "WHITE" : "BLACK"));
                    moveCount++;

                    // If now on turn white - print out notation
                    if (moveCount % 2 == 0 && moveCount > 0) {
                        System.out.format("State white " + whiteState + ", black " + blackState);
                        Move whiteMove = game.getHistory().get(game.getHistory().size() - 2); // White move was second to last
                        Move blackMove = game.getLastMove(); // Black move was last
                        String turnNotation = parseMoveToString(whiteMove, blackMove, (moveCount + 1) / 2);
                        System.out.println(turnNotation);

                        movesListView.getItems().add(turnNotation);

                        // Set highlight
                        movesListView.getSelectionModel().selectLast();
                    }


                } else {
                    // Abort move
                    abortMove(((ImageView) (t.getSource())), selectedBox);
                }

                selectedBox = null;
            }
        }
    };

    public void checkState() {
        // Warm if move caused check
        if (game.checkKingCheck((game.getTurnColor() == model.Color.WHITE) ? model.Color.BLACK : model.Color.WHITE)) {
            System.out.format("WARNING: %s king is in CHECK!\n", (game.getTurnColor() == model.Color.WHITE) ? "Black" : "White");
            if (game.getTurnColor() == model.Color.WHITE) whiteState = SpecialState.CHECK;
            else blackState = SpecialState.CHECK;
        }
        // Warm if move caused checkmate
        else if (game.checkKingCheckmate((game.getTurnColor() == model.Color.WHITE) ? model.Color.BLACK : model.Color.WHITE)) {
            System.out.format("WARNING: %s king is in CHECKMATE!\n", (game.getTurnColor() == model.Color.WHITE) ? "Black" : "White");
            if (game.getTurnColor() == model.Color.WHITE) whiteState = SpecialState.CHECKMATE;
            else blackState = SpecialState.CHECKMATE;
        }
        // Warm if move caused stalemate
        else if (game.checkKingStalemate((game.getTurnColor() == model.Color.WHITE) ? model.Color.BLACK : model.Color.WHITE)) {
            System.out.format("WARNING: %s king is in STALEMATE!\n", (game.getTurnColor() == model.Color.WHITE) ? "Black" : "White");
            if (game.getTurnColor() == model.Color.WHITE) whiteState = SpecialState.STALEMATE;
            else blackState = SpecialState.STALEMATE;
        }
        else {
            if (game.getTurnColor() == model.Color.WHITE) whiteState = SpecialState.NONE;
            else blackState = SpecialState.NONE;
        }
    }

    /**
     *
     * @param number
     * @return
     */
    public static int roundToNearest40(int number) {
        int remainder = number % 40;
        if (remainder >= 20) // Round up
            return (number / 40) * 40 + 40;
        else // Round down
            return (number / 40) * 40;
    }

    /**
     *
     * @param y
     * @return
     */
    public static int decodeRow(int y) {
        if (y < 40 || y > 360) return - 1; // Out of range
        else {
            if (y == 40) return 7;
            else if (y == 80) return 6;
            else if (y == 120) return 5;
            else if (y == 160) return 4;
            else if (y == 200) return 3;
            else if (y == 240) return 2;
            else if (y == 280) return 1;
            else return 0;
        }
    }

    /**
     *
     * @param x
     * @return
     */
    public static int decodeCol(int x) {
        if (x < 40 || x > 360) return - 1; // Out of range
        else {
            if (x == 40) return 0;
            else if (x == 80) return 1;
            else if (x == 120) return 2;
            else if (x == 160) return 3;
            else if (x == 200) return 4;
            else if (x == 240) return 5;
            else if (x == 280) return 6;
            else return 7;
        }
    }


    //returns false if error

    /**
     *
     * @param type1
     * @param fX
     * @param fY
     * @param captures
     * @param tX
     * @param tY
     * @param type2
     * @param check
     * @param checkmate
     * @return
     */
    private boolean moveFigure ( char type1, int fX, int fY, boolean captures, int tX, int tY, char type2,
                                 boolean check, boolean checkmate){
        Box selectedBox = game.getBoard().getBox(fY, fX);
        Box newBox = game.getBoard().getBox(tY, tX);
        model.Color selectedPieceColor = selectedBox.getPiece().getColor();
        if (selectedBox.getPiece().getValidMoveBoxes().contains(newBox)) {
            Move move = selectedBox.getPiece().moveToPosition(newBox);
            if (move.getCapturedPiece() != null) {
                if (!captures) {
                    return false;
                }
                Piece captured = move.getCapturedPiece();
                removeImg(captured, newBox);
            } else {
                if (captures) {
                    return false;
                }
            }

            // Moving figure image
            ImageView figureView = null;
            if (selectedPieceColor == model.Color.WHITE) {
                for (ImageView img : whiteFigures) {
                    int row = decodeRow((int) img.getY());
                    int col = decodeCol((int) img.getX());
                    if (row == selectedBox.getRow() && col == selectedBox.getCol()) {
                        figureView = img;
                        break;
                    }
                }
            } else {
                for (ImageView img : blackFigures) {
                    int row = decodeRow((int) img.getY());
                    int col = decodeCol((int) img.getX());
                    if (row == selectedBox.getRow() && col == selectedBox.getCol()) {
                        figureView = img;
                        break;
                    }
                }
            }

            setDownFigure(figureView, tX, tY);

            // Warm if move caused check
            if (game.checkKingCheck((game.getTurnColor() == model.Color.WHITE) ? model.Color.BLACK : model.Color.WHITE)) {
                if (!check) {
                    return false;
                }
                System.out.format("WARNING: %s king is in CHECK!\n", (game.getTurnColor() == model.Color.WHITE) ? "Black" : "White");
            } else {
                if (check) {
                    return false;
                }
            }

            // Warm if move caused checkmate
            if (game.checkKingCheckmate((game.getTurnColor() == model.Color.WHITE) ? model.Color.BLACK : model.Color.WHITE)) {
                if (!checkmate) {
                    return false;
                }
                System.out.format("WARNING: %s king is in CHECKMATE!\n", (game.getTurnColor() == model.Color.WHITE) ? "Black" : "White");
            } else {
                if (checkmate) {
                    return false;
                }
            }

            // Warm if move caused stalemate
            if (game.checkKingStalemate((game.getTurnColor() == model.Color.WHITE) ? model.Color.BLACK : model.Color.WHITE)) {
                System.out.format("WARNING: %s king is in STALEMATE!\n", (game.getTurnColor() == model.Color.WHITE) ? "Black" : "White");
            }

            //turnNo++;
            game.printBoard();

            // Update turn
            game.updateTurnColor();
            onTurnLabel.setText("On turn: " + ((game.getTurnColor() == model.Color.WHITE) ? "WHITE" : "BLACK"));
        } else {
            return false;
        }
        return true;
    }


    /**
     *
     * @param moves
     * @param index
     */
    public void makeMoves (List< String > moves, int index){
        Pattern r_long = Pattern.compile("^([KDVSJp])?([a-h])([1-8])(x)?([a-h])([1-8])([KDVSJp])?([+#])?$");
        Pattern r_short = Pattern.compile("^([KDVSJp])?([a-h]|[1-8])?(x)?([a-h])([1-8])([KDVSJp])?([+#])?$");
        boolean longNotation = true;
        Matcher m = r_short.matcher(moves.get(0));
        if (m.find()) {
            longNotation = false;
        }
        if (longNotation) {
            for (int i = 0; i < index; i++) {
                m = r_long.matcher(moves.get(i));
                if (!m.find()) {
                    break;
                }
                moveFigure(m.group(1) != null ? m.group(1).charAt(0) : 'p', m.group(2).charAt(0) - 'a', Integer.parseInt(m.group(3)) - 1, m.group(4) != null && m.group(4).equals("x"), m.group(5).charAt(0) - 'a', Integer.parseInt(m.group(6)) - 1, m.group(7) != null ? m.group(7).charAt(0) : ' ', m.group(8) != null && m.group(8).equals("+"), m.group(8) != null && m.group(8).equals("#"));
            }
        }
    }

    /**
     *
     * @return
     */
    public Tab getView() {
        return this.tab;
    }

    /**
     *
     * @param capturedPiece
     * @param endBox
     */
    public void removeImg(Piece capturedPiece, Box endBox) {
        if (capturedPiece != null) {
            if (capturedPiece.getColor() == model.Color.WHITE) {
                for (ImageView whitePieceImage : whiteFigures) {
                    int row = decodeRow((int) whitePieceImage.getY());
                    int col = decodeCol((int) whitePieceImage.getX());
                    if (row == endBox.getRow() && col == endBox.getCol()) {
                        groupChessBoard.getChildren().remove(whitePieceImage);
                        groupRemovedPieces.getChildren().add(whitePieceImage);

                        whitePieceImage.setX(0);
                        whitePieceImage.setY(whiteRemoved*40);
                        whitePieceImage.relocate(0, whiteRemoved*40);

                        // Add to HashMap
                        onBoardMap.remove(capturedPiece);
                        removedMap.put(capturedPiece, whitePieceImage);

                        break;
                    }
                }
                game.getBoard().getPieceSet(model.Color.WHITE).removePiece(capturedPiece);
                whiteRemoved++;

            } else {
                for (ImageView blackPieceImage : blackFigures) {
                    int row = decodeRow((int) blackPieceImage.getY());
                    int col = decodeCol((int) blackPieceImage.getX());
                    if (row == endBox.getRow() && col == endBox.getCol()) {
                        groupChessBoard.getChildren().remove(blackPieceImage);
                        groupRemovedPieces.getChildren().add(blackPieceImage);

                        blackPieceImage.setX(40);
                        blackPieceImage.setY(blackRemoved*40);
                        blackPieceImage.relocate(40, blackRemoved*40);

                        // Add to HashMap
                        removedMap.put(capturedPiece, blackPieceImage);

                        break;
                    }
                }
                game.getBoard().getPieceSet(model.Color.BLACK).removePiece(capturedPiece);
                blackRemoved++;
            }
        }
    }

    /**
     *
     * @param newBox
     * @param img
     */
    public void promotePawn(Box newBox, ImageView img) {
        String figureLoc;
        if ((newBox.getPiece().getColor() == model.Color.WHITE && newBox.getRow() == 7 && newBox.getPiece() instanceof Pawn) ||
                (newBox.getRow() == 0 && newBox.getPiece() instanceof Pawn)) {
            // Alert box to choose new figure
            figureLoc = PromotionAlertBox.display(newBox.getPiece().getColor());

            // Change image
            img.setImage(new Image(NewGame.class.getClassLoader().getResource(figureLoc).toString()));
            // Change model
            // Remove piece from chessboard
            if (newBox.getPiece().getColor() == model.Color.WHITE) {
                game.getBoard().getPieceSet(model.Color.WHITE).getPieces().remove(newBox.getPiece()); // Remove totally from model
                // Create new figure
                if (figureLoc.equals("whiteRook.png")) {
                    Rook newRook = new Rook(model.Color.WHITE);
                    game.getBoard().getPieceSet(model.Color.WHITE).getPieces().add(newRook);
                    newRook.setPosition(newBox);
                    newBox.setPiece(newRook);
                }
                else if (figureLoc.equals("whiteHorse.png")) {
                    Horse newHorse = new Horse(model.Color.WHITE);
                    game.getBoard().getPieceSet(model.Color.WHITE).getPieces().add(newHorse);
                    newHorse.setPosition(newBox);
                    newBox.setPiece(newHorse);
                }
                else if (figureLoc.equals("whiteBishop.png")) {
                    Bishop newBishop = new Bishop(model.Color.WHITE);
                    game.getBoard().getPieceSet(model.Color.WHITE).getPieces().add(newBishop);
                    newBishop.setPosition(newBox);
                    newBox.setPiece(newBishop);
                }
                else {
                    Queen newQueen = new Queen(model.Color.WHITE);
                    game.getBoard().getPieceSet(model.Color.WHITE).getPieces().add(newQueen);
                    newQueen.setPosition(newBox);
                    newBox.setPiece(newQueen);
                }
            }
            else {
                game.getBoard().getPieceSet(model.Color.BLACK).getPieces().remove(newBox.getPiece()); // Remove totally from model
                // Create new figure
                if (figureLoc.equals("blackRook.png")) {
                    Rook newRook = new Rook(model.Color.BLACK);
                    game.getBoard().getPieceSet(model.Color.BLACK).getPieces().add(newRook);
                    newRook.setPosition(newBox);
                    newBox.setPiece(newRook);
                }
                else if (figureLoc.equals("blackHorse.png")) {
                    Horse newHorse = new Horse(model.Color.BLACK);
                    game.getBoard().getPieceSet(model.Color.BLACK).getPieces().add(newHorse);
                    newHorse.setPosition(newBox);
                    newBox.setPiece(newHorse);
                }
                else if (figureLoc.equals("blackBishop.png")) {
                    Bishop newBishop = new Bishop(model.Color.BLACK);
                    game.getBoard().getPieceSet(model.Color.BLACK).getPieces().add(newBishop);
                    newBishop.setPosition(newBox);
                    newBox.setPiece(newBishop);
                }
                else {
                    Queen newQueen = new Queen(model.Color.BLACK);
                    game.getBoard().getPieceSet(model.Color.BLACK).getPieces().add(newQueen);
                    newQueen.setPosition(newBox);
                    newBox.setPiece(newQueen);
                }
            }
        }
    }

    /**
     *
     * @param img
     * @param selectedBox
     */
    public void abortMove(ImageView img, Box selectedBox) {
        img.setTranslateX(0.0);
        img.setTranslateY(0.0);

        int oldRow = selectedBox.getRow();
        int oldCol = selectedBox.getCol();

        img.setX(40 + oldCol * 40);
        img.setY(8 * 40 - oldRow * 40);

        img.relocate(40 + oldCol * 40, 8 * 40 - oldRow * 40);
        System.out.println("Move aborted");
    }

    /**
     *
     * @param img
     * @param posOnBoardX
     * @param posOnBoardY
     */
    public void setDownFigure(ImageView img, int posOnBoardX, int posOnBoardY) {
        img.setTranslateX(0.0);
        img.setTranslateY(0.0);
        img.setX(posOnBoardX);
        img.setY(posOnBoardY);
    }

    /**
     *
     * @param img
     * @param newBox
     */
    public void setDownFigure(ImageView img, Box newBox) {
        img.setX(40 + newBox.getCol() * 40);
        img.setY(8 * 40 - newBox.getRow() * 40);
    }

    /**
     *
     * @param imgClicked
     * @return
     */
    public model.Color determineClickedCol(ImageView imgClicked) {
        model.Color clickedCol = model.Color.BLACK;
        for (ImageView img : whiteFigures) {
            if (img.equals(imgClicked)) {
                clickedCol = model.Color.WHITE;
                break;
            }
        }
        return clickedCol;
    }

    /**
     *
     * @param p
     * @return
     */
    public String getPieceType(Piece p) {
        if (p instanceof King)
            return "K";
        else if (p instanceof Queen)
            return "D";
        else if (p instanceof Bishop)
            return "S";
        else if (p instanceof Horse)
            return "J";
        else if (p instanceof Rook)
            return "V";
        else
            return "";
    }

    /**
     *
     * @param b
     * @return
     */
    public String getPosition(Box b) {
        String result = "";
        result += String.valueOf((char)('a' + b.getCol())); // Col (a-h)
        result += String.valueOf((char)('1' + b.getRow())); // Row (1-8)
        return result;
    }

    /**
     *
     * @param state
     * @return
     */
    public String getSpecialState(SpecialState state) {
        if (state == SpecialState.CHECK)
            return "+";
        else if (state == SpecialState.CHECKMATE)
            return "#";
        else
            return "";
    }

    /**
     *
     * @param whiteMove
     * @param blackMove
     * @param round
     * @return
     */
    public String parseMoveToString(Move whiteMove, Move blackMove, int round) {
        String result = round + ". ";

        // White move
        result += getPieceType(whiteMove.getMovedPiece()); // Determine piece
        result += getPosition(whiteMove.getStartBox()); // Determine start box
        result += getPosition(whiteMove.getEndBox()); // Determine end box
        result += getSpecialState(whiteState);
        result += " ";
        // Black move
        result += getPieceType(blackMove.getMovedPiece()); // Determine piece
        result += getPosition(blackMove.getStartBox()); // Determine start box
        result += getPosition(blackMove.getEndBox()); // Determine end box
        result += getSpecialState(blackState);

        return result;
    }

    public String parseSingleMoveToString(Move move, SpecialState state) {
        String result = "";
        result += getPieceType(move.getMovedPiece()); // Determine piece
        result += getPosition(move.getStartBox()); // Determine start box
        result += getPosition(move.getEndBox()); // Determine end box
        result += getSpecialState(state);

        return result;
    }
}
