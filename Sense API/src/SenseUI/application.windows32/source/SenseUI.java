import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.serial.*; 
import controlP5.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class SenseUI extends PApplet {



PFont p1, p2, mbFont;

Serial cPort;
int comPort;

int sTimeKey, dTimeKey;

boolean RUNNING;
boolean DATAIN;
boolean SEARCHING;

int randColor;

String sampleArray[];
String sampleString;

int samplePeriod = 10;

CallbackListener cb;

public void setup() {
  frameRate(200);
  
  background(bgColor);
  noStroke();
  
  cb = new CallbackListener() {
    public void controlEvent(CallbackEvent theEvent) {
      println(theEvent.getController().getId());
      }
    };

  
    
  filePath = "/data/data.txt";
 
  p1 = createFont("Arial", 20); 
  mbFont = createFont("Arial", 12);

  mainPanel = new MainPanel(this);

  println("Initializing backend");
  thread("portConstructor");

}

public void draw() {
  background(bgColor);

  fill(mbColor);  // Menu bar fill
  rect(0, 0, width, 50);// Menu bar rect

  if (SEARCHING) {
    fill(color(255));
    textSize(30);
    textAlign(CENTER, CENTER);
    text("Searching for COM Port", width/2, height-25);
  }
}

public void portConstructor() {
  SEARCHING = true;
  comPort = findPort();
  cPort = new Serial(this, Serial.list()[comPort], 192000);
  SEARCHING = false;
  RUNNING = true;
  updateFilePath();
  thread("samplerConstructor");
}

public int findPort() {
  int connectPort = 0;
  println("Searching for available COM port...");
  connectButton.setLabel("Searching");
  while (connectPort == 0) {
    for (int i = 0; i < Serial.list().length; i++) {
      if (Serial.list()[i] != null) {
        println("\n\n\n\n" + "Connected to " + Serial.list()[i]);
        connectButton.setLabel(Serial.list()[i]);
        return(i);
      }
    }
  }
  return(0);
}

public void samplerConstructor() {
  println("Initializing sampler");
  while (RUNNING) {
    getSamples();
    if (DATAIN) {
      if (RECORDING) {
        fileOutput.print(frameCount + " ");
      }
      for (int i = 0; i < totalCh; i++) {
        calcVal = PApplet.parseFloat(sampleArray[i]);
        if (RECORDING) {
          fileOutput.print(calcVal + " ");
        }
        tplotArray[i].draw(calcVal);
      }
    }
    if (RECORDING && DATAIN) {
      fileOutput.println();
    }
    delay(samplePeriod);
  }
}


public void getSamples() {
  DATAIN = false;
  cPort.write(83);
  while (cPort.available() > 0) {
    String sampleString = cPort.readStringUntil(124);
    sampleString = sampleString.substring(0, sampleString.length()-1);
    sampleArray = split(sampleString, ',');
    //println(sampleString);
    DATAIN = true;
  }
}


public void startRecording() {
  println(filePath);
  if (filePath == null) {
    println("Make sure to set a file output!");
    selectOutput("Select a file to write to:", "fileSelected");
  }
  RECORDING = true;
  fileOutput = createWriter(filePath);
  fileOutput.print("#" + " t ");
  for (int i = 1; i < numAFEs+1; i++) {
    for (int j = 1; j < numChannels+1; j++) {
      fileOutput.print("AFE" + str(i) + "_" + str(j) + "\t");
    }
  }
  fileOutput.println();
}

public void stopRecording() {
  fileOutput.flush();
  fileOutput.close();
  RECORDING = false;
}
GraphBar timeGraph;

class GraphBar {
  int yMin = 0;
  int xStart, yStart;
  int xWidth, yWidth;
  int yRange;
  int AFE, CH;
  int ichID;
  String chID;
  Chart thisChart;
  Button thisButton;
  GraphBar(int _AFE, int _CH, int xS, int yS, int xW, int yW, int yR) {
    AFE = _AFE;
    CH  = _CH;
    xStart = xS;
    yStart = yS;
    xWidth = xW;
    yWidth = yW;
    yRange = yR;
    chID = str(AFE) + str(CH);
    ichID = PApplet.parseInt(chID);
    thisChart = cp5.addChart("timeData" + chID)
      .setPosition(xStart, yStart)
      .setSize(xWidth, yWidth)
      .setRange(-yRange, yRange)
      .setView(Chart.LINE)
      .setStrokeWeight(1)
      .setColorBackground(color(150))
      .setLabel("")
      ;

    thisButton = cp5.addButton("en" + chID)
      .setPosition(xStart-50, yStart)
      .setSize(yWidth, yWidth)
      .setLabel(str(AFE) + "_" + str(CH))
      .setColorBackground(fgColor)
      .setColorLabel(color(0))
      .setId(ichID)
      .setBroadcast(true)
      .addCallback(new CallbackListener() {
      public void controlEvent(CallbackEvent event) {
        if (event.getAction() == ControlP5.ACTION_ENTER) {
          thisChart.setSize(width*5/6, height*7/8).setPosition(xStart, height/12-8);
          thisChart.bringToFront();
        }
        if (event.getAction() == ControlP5.ACTION_LEAVE) {
          thisChart.setSize(xWidth, yWidth).setPosition(xStart, yStart);
        }
      }
    }
    );

    thisChart.addDataSet("tData"+ chID);
    thisChart.setColors("tData" + chID, colorList[AFE-1][CH-1]);
  }
  public void draw(float dataVal) {
    thisChart.push("tData"+ str(AFE) + str(CH), dataVal);
  }
}


ControlP5 cp5;

MainPanel mainPanel;
MenuBar topMenu;
GraphBar[] tplotArray;

int numAFEs = 4, numChannels = 4;
int totalCh = numAFEs * numChannels;
int absoluteCount;
float calcVal;

class MainPanel {

  MainPanel(SenseUI wClass) {

    cp5 = new ControlP5(wClass);
    cp5.setAutoDraw(true);

    topMenu = new MenuBar();
    
    absoluteCount = 0;

    tplotArray = new GraphBar[numAFEs*numChannels];

    for (int i = 1; i < numAFEs+1; i++) {
      for (int j = 1; j < numChannels+1; j++) {
        tplotArray[absoluteCount] = new GraphBar(i, j, 100, 75+absoluteCount * 55, 800, 50, 1);
        absoluteCount++;
      }
    }

    println("Initialized main panel");
  }
}
Button connectButton;
Button fileButton;
Button recordButton;
Textlabel filePathLabel;

String filePath;
PrintWriter fileOutput;
boolean RECORDING = false;

class MenuBar {
  int buttonWidth, buttonHeight;
  int mButtonW, mButtonH;
  MenuBar() {
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
}

public void updateFilePath() {
  filePathLabel.setText("File: " + filePath + " ");
}

public void updateRecordButton() {
  if (RECORDING == false) {
    recordButton.setLabel("Record").setColorBackground(btColor);
  } else {
    recordButton.setLabel("Stop").setColorBackground(color(255, 0, 0));
  }
}

public void fileSelected(File selection) {
  if (selection == null) {
    println("No file selected, defaulting.");
    filePath = "/data/data.txt";
  } else {
    filePath = selection.getAbsolutePath();
    println("File selected: " + filePath);
  }
  updateFilePath();
}
// Main colors used in Sense UI

int bgColor = 0xff253237;
int wbColor = 0xff5C6B73;
int mbColor = 0xff9DB4C0;
int btColor = 0xff134074;
int fgColor = 0xffF4FCFC;

int[][] colorList = {{color(128,0,0), color(165,42,42), color(220,20,60), color(255,0,0)}, //AFE1
{color(184,134,11), color(218,165,32), color(255,215,0), color(255,255,0)}, //AFE2
{color(0,128,0), color(50,205,50), color(127,255,0), color(152,251,152)}, //AFE3
{color(0,0,205), color(0,0,255), color(65,105,225), color(135,206,250)}}; //AFE4
  public void settings() {  size(1000, 1000); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "SenseUI" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
