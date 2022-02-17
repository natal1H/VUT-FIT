package gui;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.GridPane;
import javafx.stage.Modality;
import javafx.stage.Stage;
import model.Color;

public class PromotionAlertBox {

    static String chosenFigure;

    public static String display(model.Color color) {
        Stage window = new Stage();

        // Block events to other windows
        window.initModality(Modality.APPLICATION_MODAL);
        window.setTitle("Pawn promotion");
        window.setWidth(300);

        Label label = new Label();
        label.setText("Choose figure");
        ImageView rookImg = new ImageView(new Image(NewGame.class.getClassLoader().getResource((color == Color.WHITE) ? "whiteRook.png" : "blackRook.png").toString()));
        rookImg.setFitHeight(40); rookImg.setFitWidth(40);
        ImageView horseImg = new ImageView(new Image(NewGame.class.getClassLoader().getResource((color == Color.WHITE) ? "whiteHorse.png" : "blackHorse.png").toString()));
        horseImg.setFitHeight(40); horseImg.setFitWidth(40);
        ImageView bishopImg = new ImageView(new Image(NewGame.class.getClassLoader().getResource((color == Color.WHITE) ? "whiteBishop.png" : "blackBishop.png").toString()));
        bishopImg.setFitHeight(40); bishopImg.setFitWidth(40);
        ImageView queenImg = new ImageView(new Image(NewGame.class.getClassLoader().getResource((color == Color.WHITE) ? "whiteQueen.png" : "blackQueen.png").toString()));
        queenImg.setFitHeight(40); queenImg.setFitWidth(40);

        // Create GridPane
        GridPane gridPane = new GridPane();
        gridPane.setMinSize(300, 150); //Setting size for the pane
        gridPane.setPadding(new Insets(10, 10, 10, 10)); //Setting the padding
        gridPane.setVgap(5); //Setting the vertical gap between the columns
        gridPane.setHgap(5); //Setting the horizontal gap between the columns

        gridPane.setAlignment(Pos.CENTER); //Setting the Grid alignment

        //Arranging all the nodes in the grid
        gridPane.add(label, 0, 0, 4, 1);
        gridPane.add(rookImg, 0, 1);
        gridPane.add(horseImg, 1, 1);
        gridPane.add(bishopImg, 2, 1);
        gridPane.add(queenImg, 3, 1);

        // Events
        rookImg.setOnMouseClicked(e -> {
            chosenFigure = (color == Color.WHITE) ? "whiteRook.png" : "blackRook.png";
            System.out.format("You have chosen: %s\n", chosenFigure);
            window.close();
        });
        horseImg.setOnMouseClicked(e -> {
            chosenFigure = (color == Color.WHITE) ? "whiteHorse.png" : "blackHorse.png";
            System.out.format("You have chosen: %s\n", chosenFigure);
            window.close();
        });
        bishopImg.setOnMouseClicked(e -> {
            chosenFigure = (color == Color.WHITE) ? "whiteBishop.png" : "blackBishop.png";
            System.out.format("You have chosen: %s\n", chosenFigure);
            window.close();
        });
        queenImg.setOnMouseClicked(e -> {
            chosenFigure = (color == Color.WHITE) ? "whiteQueen.png" : "blackQueen.png";
            System.out.format("You have chosen: %s\n", chosenFigure);
            window.close();
        });

        //Creating a scene object
        Scene scene = new Scene(gridPane);

        window.setScene(scene); // Set scene
        window.showAndWait(); //Displaying the contents of the stage

        return chosenFigure;
    }
}
