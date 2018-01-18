Button connectButton;
Button fileButton;
Button recordButton;
Textlabel filePathLabel;

String filePath;
PrintWriter fileOutput;
boolean RECORDING = false;

class MenuBar {
  int barWidth, barHeight;
  int buttonWidth, buttonHeight;
  int mButtonW, mButtonH;
  MenuBar() {
    barWidth  = width;
    barHeight = 50;
    mButtonW  = 80;
    mButtonH  = 40;

    cp5.setFont(mbFont);
    
    println("Initialized menu bar");

    connectButton = cp5.addButton("connectButton")
      .setLabel("Connect")
      .setSize(80, 40)
      .setPosition(5, 5)
      .addCallback(new CallbackListener() {
      public void controlEvent(CallbackEvent event) {
        if (event.getAction() == ControlP5.ACTION_RELEASED) {
          println("WHOO");
        }
      }
    }
    );
    fileButton = cp5.addButton("fileButton", 2)
      .setLabel("File")
      .setSize(80, 40)
      .setPosition(90, 5).addCallback(new CallbackListener() {
      public void controlEvent(CallbackEvent event) {
        if (event.getAction() == ControlP5.ACTION_RELEASED) {
          println("Selecting file...");
          selectOutput("Select a file to write to:", "fileSelected");
        }
      }
    }
    );

    filePathLabel = cp5.addLabel("filePathLabel")
      .setText("File: " + filePath + " ")
      .setFont(p1)
      .setPosition(175, 10);

    recordButton = cp5.addButton("recordButton", 4)
      .setLabel("Record")
      .setSize(80, 40)
      .setPosition(width-85, 5).addCallback(new CallbackListener() {
      public void controlEvent(CallbackEvent event) {
        if (event.getAction() == ControlP5.ACTION_RELEASED) {
          if (RECORDING == false) {
            startRecording();
            updateRecordButton();
          } else {
            stopRecording();
            updateRecordButton();
          }
        }
      }
    }
    );
  }

  void draw() {
    fill(mbColor);  // Menu bar fill
    rect(0, 0, barWidth, barHeight);// Menu bar rect
  }
}

void updateFilePath() {
  filePathLabel.setText("File: " + filePath + " ");
}

void updateRecordButton() {
  if (RECORDING == false) {
    recordButton.setLabel("Record").setColorBackground(btColor);
  } else {
    recordButton.setLabel("Stop").setColorBackground(color(255, 0, 0));
  }
}

void fileSelected(File selection) {
  if (selection == null) {
    println("No file selected, defaulting.");
    filePath = "/data/data.txt";
  } else {
    filePath = selection.getAbsolutePath();
    println("File selected: " + filePath);
  }
  updateFilePath();
  fileOutput = createWriter(filePath);
}