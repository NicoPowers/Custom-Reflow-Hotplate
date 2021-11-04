
import processing.serial.*;

Serial myPort;        // The serial port
Table myData;
int xPos = 1;         // horizontal position of the graph 

//Variables to draw a continuous line.
float inByte;
int lastxPos=1;
int lastheight=0;

void setup () {
  // set the window size:
  size(600, 400);        

  // List all the available serial ports
  println(Serial.list());
  // Check the listed serial ports in your machine
  // and use the correct index number in Serial.list()[].

  myPort = new Serial(this, Serial.list()[0], 115200);  //

  // A serialEvent() is generated when a newline character is received :
  myPort.bufferUntil('\n');
  background(0);      // set inital background:
}
void draw () {
  //Drawing a line from Last inByte to the new one.
    stroke(127,34,255);     //stroke color
    strokeWeight(4);        //stroke wider
    line(lastxPos, lastheight, xPos, height - inByte); 
    lastxPos= xPos;
    lastheight= int(height-inByte);

    // at the edge of the window, go back to the beginning:
    //if (xPos >= width) {
    //  xPos = 0;
    //  lastxPos= 0;
    //  background(0);  //Clear the screen.
    //} 
    //else {
    //  // increment the horizontal position:
    //  xPos++;
    //}
    xPos++;
}

void serialEvent (Serial myPort) {
  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');
  print(inString);
  
  if (inString.contains(",")){
    String data[] = split(inString, ',');
    inByte = float(data[1]);           // convert to a number.
    
    inByte = map(inByte, 0, 200, 0, height); //map to the screen height.
    
    print("received value", inByte, "\n");
    
    
  }
  
}
