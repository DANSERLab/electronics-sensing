import processing.serial.*;

PFont p1, p2, mbFont;

Serial cPort;
int comPort;

int sTimeKey, dTimeKey;

boolean RUNNING;
boolean DATAIN;
boolean SEARCHING;

color randColor;

String sampleArray[];
String sampleString;

int samplePeriod = 10; //ms


public void setup() {
  frameRate(200);
  size(1000, 1000);
  background(bgColor);
  noStroke();
  
    
  filePath = "/data/data.txt";
 
  p1 = createFont("Arial", 20); 
  mbFont = createFont("Arial", 12);

  mainPanel = new MainPanel(this);

  println("Initializing backend");
  thread("portConstructor");


}

void draw() {
  background(bgColor);

  fill(mbColor);  // Menu bar fill
  rect(0, 0, width, 50);// Menu bar rect

  if (SEARCHING) {
    fill(color(255));
    textSize(30);
    textAlign(CENTER, CENTER);
    text("Searching for COM Port", width/2, height/2);
  }
}

void portConstructor() {
  SEARCHING = true;
  comPort = findPort();
  cPort = new Serial(this, Serial.list()[comPort], 192000);
  SEARCHING = false;
  RUNNING = true;
  updateFilePath();
  thread("samplerConstructor");
}

int findPort() {
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

void samplerConstructor() {
  println("Initializing sampler");
  while (RUNNING) {
    getSamples();
    if (DATAIN) {
      if (RECORDING) {
        fileOutput.print(frameCount + " ");
      }
      for (int i = 0; i < totalCh; i++) {
        calcVal = float(sampleArray[i]);
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


void getSamples() {
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


void startRecording() {
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

void stopRecording() {
  fileOutput.flush();
  fileOutput.close();
  RECORDING = false;
}