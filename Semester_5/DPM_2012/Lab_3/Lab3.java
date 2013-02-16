import lejos.nxt.Button;
import lejos.nxt.LCD;
import lejos.nxt.Motor;


public class Lab3 {

	public static void main(String[] args){
		Odometer odometer = new Odometer();
		OdometryDisplay odometryDisplay = new OdometryDisplay(odometer);
		Navigator navigator;
		int buttonChoice;
		
		do {
			// clear the display
			LCD.clear();

			//Ask whether to run it with or without an obstacle.
			LCD.drawString("< Left | Right >", 0, 0);
			LCD.drawString("       |        ", 0, 1);
			LCD.drawString("  W/O  |  W/  ", 0, 2);
			LCD.drawString(" obst. | obst.   ", 0, 3);

			buttonChoice = Button.waitForAnyPress();
		} while (buttonChoice != Button.ID_LEFT
				&& buttonChoice != Button.ID_RIGHT);
		
		if(buttonChoice == Button.ID_LEFT){
			//Store the path to follow when an obstacle is not there in an array. 
			//An object is created for each pair of coordinate points.
			Coordinates[]coordinatesArr = new Coordinates[4];
			Coordinates coordinate1 = new Coordinates(30,60);
			Coordinates coordinate2 = new Coordinates(30,30);
			Coordinates coordinate3 = new Coordinates(60,30);
			Coordinates coordinate4 = new Coordinates(0,60);
			coordinatesArr[0] = coordinate1;
			coordinatesArr[1] = coordinate2;
			coordinatesArr[2] = coordinate3;
			coordinatesArr[3] = coordinate4;
			
			//Start the odometer and navigate to the destination.
			odometer.start();
			odometryDisplay.start();
			navigator = new Navigator(odometer,Motor.A,Motor.B,coordinatesArr,Navigator.NAVIGATE_WO_OBSTACLE);
			navigator.start();
		}else{
			//Store the path to follow when an obstacle is there in an array. 
			//An object is created for each pair of coordinate points.
			Coordinates[]coordinatesArr = new Coordinates[2];
			Coordinates coordinate1 = new Coordinates(60,0);
			Coordinates coordinate2 = new Coordinates(0,60);
			coordinatesArr[0] = coordinate1;
			coordinatesArr[1] = coordinate2;
	
			//Start the odometer and navigate to the destination.
			odometer.start();
			odometryDisplay.start();
			navigator = new Navigator(odometer,Motor.A,Motor.B,coordinatesArr,Navigator.NAVIGATE_W_OBSTACLE);
			navigator.start();
		}
		
		while (Button.waitForAnyPress() != Button.ID_ESCAPE);
		System.exit(0);
	}
}
