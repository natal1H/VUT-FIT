from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QFileDialog
from PyQt5.QtGui import QImage
import cv2  # Needs opencv-python-headless, opencv-contrib-python-headless
import imutils

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("VINeffects")
        MainWindow.resize(792, 542)
        MainWindow.setMinimumSize(QtCore.QSize(0, 0))
        MainWindow.setMaximumSize(QtCore.QSize(800, 640))
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.imageLabel = QtWidgets.QLabel(self.centralwidget)
        self.imageLabel.setGeometry(QtCore.QRect(10, 10, 500, 500))
        self.imageLabel.setMaximumSize(QtCore.QSize(500, 500))
        self.imageLabel.setText("")
        self.imageLabel.setAlignment(QtCore.Qt.AlignCenter)
        self.imageLabel.setObjectName("imageLabel")
        self.groupBoxEffects = QtWidgets.QGroupBox(self.centralwidget)
        self.groupBoxEffects.setGeometry(QtCore.QRect(540, 70, 221, 391))
        self.groupBoxEffects.setObjectName("groupBox")
        self.layoutWidget = QtWidgets.QWidget(self.groupBoxEffects)
        self.layoutWidget.setGeometry(QtCore.QRect(10, 30, 185, 170))
        self.layoutWidget.setObjectName("layoutWidget")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout.setObjectName("verticalLayout")
        self.radioNone = QtWidgets.QRadioButton(self.layoutWidget)
        self.radioNone.setObjectName("radioButton")
        self.verticalLayout.addWidget(self.radioNone)
        self.radioOil = QtWidgets.QRadioButton(self.layoutWidget)
        self.radioOil.setObjectName("radioButton_2")
        self.verticalLayout.addWidget(self.radioOil)
        self.radioWatercolor = QtWidgets.QRadioButton(self.layoutWidget)
        self.radioWatercolor.setObjectName("radioButton_3")
        self.verticalLayout.addWidget(self.radioWatercolor)
        self.radioBWSketch = QtWidgets.QRadioButton(self.layoutWidget)
        self.radioBWSketch.setObjectName("radioButton_4")
        self.verticalLayout.addWidget(self.radioBWSketch)
        self.radioColorSketch = QtWidgets.QRadioButton(self.layoutWidget)
        self.radioColorSketch.setObjectName("radioButton_5")
        self.verticalLayout.addWidget(self.radioColorSketch)
        self.groupBoxParams = QtWidgets.QGroupBox(self.groupBoxEffects)
        self.groupBoxParams.setGeometry(QtCore.QRect(10, 210, 191, 161))
        self.groupBoxParams.setObjectName("groupBox_2")
        self.widget = QtWidgets.QWidget(self.groupBoxParams)
        self.widget.setGeometry(QtCore.QRect(10, 30, 171, 128))
        self.widget.setObjectName("widget")
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(self.widget)
        self.verticalLayout_2.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.labelParam1 = QtWidgets.QLabel(self.widget)
        self.labelParam1.setObjectName("labelParam1")
        self.verticalLayout_2.addWidget(self.labelParam1)
        self.sliderParam1 = QtWidgets.QSlider(self.widget)
        self.sliderParam1.setOrientation(QtCore.Qt.Horizontal)
        self.sliderParam1.setObjectName("sliderParam1")
        self.verticalLayout_2.addWidget(self.sliderParam1)
        self.labelParam2 = QtWidgets.QLabel(self.widget)
        self.labelParam2.setObjectName("labelParam2")
        self.verticalLayout_2.addWidget(self.labelParam2)
        self.sliderParam2 = QtWidgets.QSlider(self.widget)
        self.sliderParam2.setOrientation(QtCore.Qt.Horizontal)
        self.sliderParam2.setObjectName("sliderParam2")
        self.verticalLayout_2.addWidget(self.sliderParam2)
        self.labelParam3 = QtWidgets.QLabel(self.widget)
        self.labelParam3.setObjectName("labelParam3")
        self.verticalLayout_2.addWidget(self.labelParam3)
        self.sliderParam3 = QtWidgets.QSlider(self.widget)
        self.sliderParam3.setOrientation(QtCore.Qt.Horizontal)
        self.sliderParam3.setObjectName("sliderParam3")
        self.verticalLayout_2.addWidget(self.sliderParam3)
        self.layoutWidget1 = QtWidgets.QWidget(self.centralwidget)
        self.layoutWidget1.setGeometry(QtCore.QRect(540, 20, 221, 41))
        self.layoutWidget1.setObjectName("layoutWidget1")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.layoutWidget1)
        self.horizontalLayout.setContentsMargins(0, 0, 0, 0)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.openButton = QtWidgets.QPushButton(self.layoutWidget1)
        self.openButton.setObjectName("openButton")
        self.horizontalLayout.addWidget(self.openButton)
        self.saveButton = QtWidgets.QPushButton(self.layoutWidget1)
        self.saveButton.setObjectName("saveButton")
        self.horizontalLayout.addWidget(self.saveButton)
        MainWindow.setCentralWidget(self.centralwidget)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

        # Variables
        self.imageFilename = None # Holds image filename
        self.originalImage = None # Holds original image for display
        self.currentImage = None # Holds current image for display

        # Actions
        self.openButton.clicked.connect(self.loadImage)
        self.saveButton.clicked.connect(self.saveImage)
        self.radioNone.clicked.connect(self.createNoneEffect)
        self.radioOil.clicked.connect(self.createOilEffect)
        self.radioWatercolor.clicked.connect(self.createWatercolorEffect)
        self.radioBWSketch.clicked.connect(self.createBWSketchEffect)
        self.radioColorSketch.clicked.connect(self.createColorSketchEffect)

        # sliders changed actions
        self.sliderParam1.sliderReleased.connect(self.slider1Changed)
        self.sliderParam1.sliderMoved.connect(self.slider1Moved)
        self.sliderParam2.sliderReleased.connect(self.slider2Changed)
        self.sliderParam2.sliderMoved.connect(self.slider1Moved)
        self.sliderParam3.sliderReleased.connect(self.slider3Changed)
        self.sliderParam3.sliderMoved.connect(self.slider1Moved)

        self.groupBoxParams.hide()  # Hide params groupbox
        self.radioNone.setChecked(True)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("VINeffects", "VINeffects"))
        self.groupBoxEffects.setTitle(_translate("VINeffects", "Effects"))
        self.radioNone.setText(_translate("VINeffects", "None"))
        self.radioOil.setText(_translate("VINeffects", "Oil painting"))
        self.radioWatercolor.setText(_translate("VINeffects", "Watercolor painting"))
        self.radioBWSketch.setText(_translate("VINeffects", "Black and White Sketch"))
        self.radioColorSketch.setText(_translate("VINeffects", "Colored Sketch"))
        self.groupBoxParams.setTitle(_translate("VINeffects", "Parameters"))
        self.labelParam1.setText(_translate("VINeffects", "Param1"))
        self.labelParam2.setText(_translate("VINeffects", "Param2"))
        self.labelParam3.setText(_translate("VINeffects", "Param3"))
        self.openButton.setText(_translate("VINeffects", "Open"))
        self.saveButton.setText(_translate("VINeffects", "Save"))

    def loadImage(self):
        """ This function will load the user selected image
            and set it to label using the setPhoto function
        """
        self.imageFilename = QFileDialog.getOpenFileName(filter="Image files (*.jpg *.png)")[0]
        # handle cancelling open
        if self.imageFilename == "":
            return

        self.originalImage = cv2.imread(self.imageFilename)
        self.currentImage = self.originalImage
        self.setImage(self.originalImage)
        self.radioNone.setChecked(True)

    def setImage(self, image):
        """ This function will take image input and resize it
            only for display purpose and convert it to QImage
            to set at the label.
        """
        if image is None:
            return

        height, width, channels = image.shape
        if height < width:
            image = imutils.resize(image, width=500)
        else:
            image = imutils.resize(image, height=500)
        frame = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        image = QImage(frame, frame.shape[1], frame.shape[0], frame.strides[0], QImage.Format_RGB888)
        self.imageLabel.setPixmap(QtGui.QPixmap.fromImage(image))

    def setImageBW(self, image):
        if image is None:
            return

        height, width = image.shape
        if width > height:
            image = imutils.resize(image, width=500)
        else:
            image = imutils.resize(image, height=500)
        frame = cv2.cvtColor(image, cv2.COLOR_GRAY2RGB)
        image = QImage(frame, frame.shape[1], frame.shape[0], frame.strides[0], QImage.Format_RGB888)
        self.imageLabel.setPixmap(QtGui.QPixmap.fromImage(image))

    def createNoneEffect(self):
        if self.checkIfImagePresent():
            self.currentImage = self.originalImage
            self.setImage(self.currentImage)

            # hide params sliders
            self.groupBoxParams.hide()

    def createOilEffect(self):
        if self.checkIfImagePresent():
            # show params sliders - need only 2
            self.groupBoxParams.show()
            self.sliderParam3.hide()
            self.labelParam3.hide()
            # configure param slider 1
            self.sliderParam1.setMinimum(1)
            self.sliderParam1.setMaximum(20)
            self.sliderParam1.setValue(7)
            self.labelParam1.setText("size: " + str(self.sliderParam1.value()))

            # configure param slider 2
            self.sliderParam2.setMinimum(1)
            self.sliderParam2.setMaximum(10)
            self.sliderParam2.setValue(1)
            self.labelParam2.setText("dynRatio: " + str(self.sliderParam2.value()))

            # perform the oil effect
            self.currentImage = cv2.xphoto.oilPainting(self.originalImage, self.sliderParam1.value(), self.sliderParam2.value())
            self.setImage(self.currentImage)

    def createWatercolorEffect(self):
        if self.checkIfImagePresent():
            # show params sliders - need only 2
            self.groupBoxParams.show()
            self.sliderParam3.hide()
            self.labelParam3.hide()
            # configure param slider 1
            self.sliderParam1.setMinimum(0)
            self.sliderParam1.setMaximum(200)
            self.sliderParam1.setValue(60)
            self.labelParam1.setText("sigma s: " + str(self.sliderParam1.value()))

            # configure param slider 2
            self.sliderParam2.setRange(0, 10)
            self.sliderParam2.setSingleStep(1)
            self.sliderParam2.setValue(6)
            self.labelParam2.setText("sigma r: " + str(self.sliderParam2.value() / 10.0))

            # perform the watercolor effect
            self.currentImage = cv2.stylization(self.originalImage, sigma_s=self.sliderParam1.value(), sigma_r=self.sliderParam2.value() / 10.0)
            self.setImage(self.currentImage)

    def createBWSketchEffect(self):
        if self.checkIfImagePresent():
            # show params sliders - need 3
            self.groupBoxParams.show()

            # configure param slider 1
            self.sliderParam1.setMinimum(0)
            self.sliderParam1.setMaximum(200)
            self.sliderParam1.setValue(60)
            self.labelParam1.setText("sigma s: " + str(self.sliderParam1.value()))

            # configure param slider 2
            self.sliderParam2.setRange(0, 100)
            self.sliderParam2.setSingleStep(1)
            self.sliderParam2.setValue(7)
            self.labelParam2.setText("sigma r: " + str(self.sliderParam2.value() / 100.0))

            # configure param slider 3
            self.sliderParam3.show()
            self.labelParam3.show()
            self.sliderParam3.setRange(0, 20)
            self.sliderParam3.setSingleStep(1)
            self.sliderParam3.setValue(1)
            self.labelParam3.setText("shade factor: " + str(self.sliderParam3.value() / 20.0))

            self.currentImage, _ = cv2.pencilSketch(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                    sigma_r=self.sliderParam2.value() / 100.0, shade_factor=self.sliderParam3.value() / 20.0)
            self.setImageBW(self.currentImage)

    def createColorSketchEffect(self):
        if self.checkIfImagePresent():
            # show params sliders - need 3
            self.groupBoxParams.show()

            # configure param slider 1
            self.sliderParam1.setMinimum(0)
            self.sliderParam1.setMaximum(200)
            self.sliderParam1.setValue(60)
            self.labelParam1.setText("sigma s: " + str(self.sliderParam1.value()))

            # configure param slider 2
            self.sliderParam2.setRange(0, 100)
            self.sliderParam2.setSingleStep(1)
            self.sliderParam2.setValue(7)
            self.labelParam2.setText("sigma r: " + str(self.sliderParam2.value() / 100.0))

            # configure param slider 3
            self.sliderParam3.show()
            self.labelParam3.show()
            self.sliderParam3.setRange(0, 20)
            self.sliderParam3.setSingleStep(1)
            self.sliderParam3.setValue(1)
            self.labelParam3.setText("shade factor: " + str(self.sliderParam3.value() / 20.0))

            _, self.currentImage = cv2.pencilSketch(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                    sigma_r=self.sliderParam2.value() / 100.0,
                                                    shade_factor=self.sliderParam3.value() / 20.0)
            self.setImage(self.currentImage)

    def saveImage(self):
        """ This function will save the image"""
        if self.currentImage is not None:
            filename = QFileDialog.getSaveFileName(filter="JPG(*.jpg);;PNG(*.png)")[0]
            # handle cancelling saving
            if filename == "":
                return

            cv2.imwrite(filename, self.currentImage)

    def checkIfImagePresent(self):
        if self.originalImage is None:
            self.radioNone.setChecked(True)
            return False
        else:
            return True

    def slider1Changed(self):
        if self.radioOil.isChecked():
            self.labelParam1.setText("size: " + str(self.sliderParam1.value()))
            self.currentImage = cv2.xphoto.oilPainting(self.originalImage, self.sliderParam1.value(),
                                                       self.sliderParam2.value())
            self.setImage(self.currentImage)
        elif self.radioWatercolor.isChecked():
            self.labelParam1.setText("sigma s: " + str(self.sliderParam1.value()))
            self.currentImage = cv2.stylization(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                sigma_r=self.sliderParam2.value() / 10.0)
            self.setImage(self.currentImage)
        elif self.radioBWSketch.isChecked():
            self.labelParam1.setText("sigma s: " + str(self.sliderParam1.value()))
            self.currentImage, _ = cv2.pencilSketch(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                    sigma_r=self.sliderParam2.value() / 100.0,
                                                    shade_factor=self.sliderParam3.value() / 20.0)
            self.setImageBW(self.currentImage)
        elif self.radioColorSketch.isChecked():
            self.labelParam1.setText("sigma s: " + str(self.sliderParam1.value()))
            _, self.currentImage = cv2.pencilSketch(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                    sigma_r=self.sliderParam2.value() / 100.0,
                                                    shade_factor=self.sliderParam3.value() / 20.0)
            self.setImage(self.currentImage)

    def slider2Changed(self):
        if self.radioOil.isChecked():
            self.labelParam2.setText("dynRatio: " + str(self.sliderParam2.value()))
            self.currentImage = cv2.xphoto.oilPainting(self.originalImage, self.sliderParam1.value(),
                                                       self.sliderParam2.value())
            self.setImage(self.currentImage)
        elif self.radioWatercolor.isChecked():
            self.labelParam2.setText("sigma r: " + str(self.sliderParam2.value() / 10.0))
            self.currentImage = cv2.stylization(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                sigma_r=self.sliderParam2.value() / 10.0)
            self.setImage(self.currentImage)
        elif self.radioBWSketch.isChecked():
            self.labelParam2.setText("sigma r: " + str(self.sliderParam2.value() / 100.0))
            self.currentImage, _ = cv2.pencilSketch(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                    sigma_r=self.sliderParam2.value() / 100.0,
                                                    shade_factor=self.sliderParam3.value() / 20.0)
            self.setImageBW(self.currentImage)
        elif self.radioColorSketch.isChecked():
            self.labelParam2.setText("sigma r: " + str(self.sliderParam2.value() / 100.0))
            _, self.currentImage = cv2.pencilSketch(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                    sigma_r=self.sliderParam2.value() / 100.0,
                                                    shade_factor=self.sliderParam3.value() / 20.0)
            self.setImage(self.currentImage)

    def slider3Changed(self):
        if self.radioBWSketch.isChecked():
            self.labelParam3.setText("shade factor: " + str(self.sliderParam3.value() / 20.0))
            self.currentImage, _ = cv2.pencilSketch(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                    sigma_r=self.sliderParam2.value() / 100.0,
                                                    shade_factor=self.sliderParam3.value() / 20.0)
            self.setImageBW(self.currentImage)
        elif self.radioColorSketch.isChecked():
            self.labelParam3.setText("shade factor: " + str(self.sliderParam3.value() / 20.0))
            _, self.currentImage = cv2.pencilSketch(self.originalImage, sigma_s=self.sliderParam1.value(),
                                                    sigma_r=self.sliderParam2.value() / 100.0,
                                                    shade_factor=self.sliderParam3.value() / 20.0)
            self.setImage(self.currentImage)

    def slider1Moved(self):
        if self.radioOil.isChecked():
            self.labelParam1.setText("size: " + str(self.sliderParam1.value()))
        elif self.radioWatercolor.isChecked():
            self.labelParam1.setText("sigma s: " + str(self.sliderParam1.value()))
        elif self.radioBWSketch.isChecked():
            self.labelParam1.setText("sigma s: " + str(self.sliderParam1.value()))
        elif self.radioColorSketch.isChecked():
            self.labelParam1.setText("sigma s: " + str(self.sliderParam1.value()))

    def slider2Moved(self):
        if self.radioOil.isChecked():
            self.labelParam2.setText("dynRatio: " + str(self.sliderParam2.value()))
        elif self.radioWatercolor.isChecked():
            self.labelParam2.setText("sigma r: " + str(self.sliderParam2.value() / 10.0))
        elif self.radioBWSketch.isChecked():
            self.labelParam2.setText("sigma r: " + str(self.sliderParam2.value() / 100.0))
        elif self.radioColorSketch.isChecked():
            self.labelParam2.setText("sigma r: " + str(self.sliderParam2.value() / 100.0))

    def slider3Moved(self):
        if self.radioBWSketch.isChecked():
            self.labelParam3.setText("shade factor: " + str(self.sliderParam3.value() / 20.0))
        elif self.radioColorSketch.isChecked():
            self.labelParam3.setText("shade factor: " + str(self.sliderParam3.value() / 20.0))
