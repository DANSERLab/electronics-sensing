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
    ichID = int(chID);
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
  void draw(float dataVal) {
    thisChart.push("tData"+ str(AFE) + str(CH), dataVal);
  }
}