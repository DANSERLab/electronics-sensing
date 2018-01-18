import processing.serial.*;
import toxi.util.events.*;

PFont p1, p2, mbFont;

Serial cPort;
int comPort;

int sTimeKey, dTimeKey;
boolean RUNNING;
boolean DATAIN;

color randColor;

DemoEventBroadcaster ticker;
BgUpdater bg;

String sampleArray[];
String sampleString;
int lf=10;


public void setup() {
  frameRate(200);
  size(1000, 1000);
  background(bgColor);
  noStroke();

  bg = new BgUpdater();
  ticker = new DemoEventBroadcaster(10);

  comPort = findPort();
  cPort = new Serial(this, Serial.list()[comPort], 192000);

  p1 = createFont("Arial", 20); 
  mbFont = createFont("Arial", 12);

  mainPanel = new MainPanel(this);
  RUNNING = true;

  ticker.dispatcher.addListener(bg);
}

void draw() {

  ticker.update();
}

interface DemoEventListener {
  void tick();
}


class BgUpdater implements DemoEventListener {
  void tick() {
    randColor = (int)random(0xffffff);
    if (millis() - dTimeKey >= 30) {
      dTimeKey = millis();
      mainPanel.draw();
    }
    if (millis() - sTimeKey >= 5) {
      getSamples();
      sTimeKey = millis();
    }
  }
}

void getSamples() {
  DATAIN = false;
  cPort.write(83);
  while (cPort.available() > 0) {
    String sampleString = cPort.readStringUntil(124);
    println(sampleString);
    sampleString = sampleString.substring(0,sampleString.length()-1);
    sampleArray = split(sampleString, ',');
    DATAIN = true;
  }
}


int findPort() {
  println("Searching for available com port from list:");
  println(Serial.list());
  for (int i = 0; i < Serial.list().length; i++) {
    if (Serial.list()[i] != null) {
      println("\n\n\n\n" + "Connected to " + Serial.list()[i]);
      return(i);
    }
  }
  delay(1);
  findPort();
  return(0);
}

void startRecording() {
  println(filePath);
  if (filePath == null) {
    println("Make sure to set a file output!");
    selectOutput("Select a file to write to:", "fileSelected");
  }
  RECORDING = true;
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


class DemoEventBroadcaster {

  public EventDispatcher<DemoEventListener> dispatcher=new EventDispatcher<DemoEventListener>();

  long lastEvent;
  long period;

  DemoEventBroadcaster(long period) {
    this.lastEvent=System.currentTimeMillis();
    this.period=period;
  }

  void update() {
    long now=System.currentTimeMillis();
    if (now-lastEvent>=period) {
      for (DemoEventListener l : dispatcher) {
        l.tick();
      }
      lastEvent=now;
    }
  }
}