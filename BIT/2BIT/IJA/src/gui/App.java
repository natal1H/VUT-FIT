package gui;

import javafx.application.Application;
import javafx.scene.Scene;
import javafx.scene.layout.*;
import javafx.stage.Stage;
import javafx.scene.Group;
import javafx.scene.control.*;
import javafx.event.Event;
import javafx.event.EventHandler;
import model.*;

import java.util.ArrayList;

public class App extends Application {

    ArrayList<NewGame> listOfGames = new ArrayList<>();
    int tabCounter = 0;

    // launch the application
    public void start(Stage stage)
    {

        // set title for the stage
        stage.setTitle("Chess");

        // create a tabpane
        TabPane tabpane = new TabPane();

        // create a tab which
        // when pressed creates a new tab
        Tab newtab = new Tab("+");

        // action event
        EventHandler<Event> event = e -> {
            if (newtab.isSelected())
            {
                listOfGames.add(new NewGame(tabCounter));
                Tab tab = listOfGames.get(tabCounter).getView();
                tabCounter++;

                // add tab
                tabpane.getTabs().add(
                        tabpane.getTabs().size() - 1, tab);

                // select the last tab
                tabpane.getSelectionModel().select(
                        tabpane.getTabs().size() - 2);
            }
        };

        // set event handler to the tab
        newtab.setOnSelectionChanged(event);

        // add newtab
        tabpane.getTabs().add(newtab);

        // create a scene
        Scene scene = new Scene(tabpane, 800, 600);

        // set the scene
        stage.setScene(scene);

        stage.show();
    }

    public static void setupGUI(String[] args) {
        launch(args);
    }
}