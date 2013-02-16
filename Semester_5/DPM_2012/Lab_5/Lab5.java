import lejos.nxt.Button;
import lejos.nxt.LCD;
import lejos.nxt.LightSensor;
import lejos.nxt.Motor;
import lejos.nxt.SensorPort;
import lejos.nxt.UltrasonicSensor;
import lejos.nxt.comm.RConsole;

public class Lab5 {
	public static void main(String[] args) {
		RConsole.openBluetooth(1000);
		int buttonChoice;
		
		do {
			// Ask whether to run tests or find the light source.
			LCD.drawString("< Left | Right >", 0, 0);
			LCD.drawString("       |        ", 0, 1);
			LCD.drawString("  Run  | Find   ", 0, 2);
			LCD.drawString("  Test | Beacon ", 0, 3);
			buttonChoice = Button.waitForAnyPress();
		} while (buttonChoice != Button.ID_LEFT
				&& buttonChoice != Button.ID_RIGHT);

		if (buttonChoice == Button.ID_LEFT) {//The left button causes the robot to perform the tests.
			UltrasonicSensor us = new UltrasonicSensor(SensorPort.S1);
			LightSensor ls = new LightSensor(SensorPort.S2);
			TwoWheeledRobot patBot = new TwoWheeledRobot(Motor.A, Motor.B);
			Odometer odo = new Odometer(patBot, true);
			LCDInfo lcd = new LCDInfo(odo);
			Navigation nav = new Navigation(odo);
			LightFinder lf = new LightFinder(odo, us, ls);
			lf.findLightSourceHeading();
			lf.reset();
			lf.findLightSourceHeading();
			lf.findLightSourceHeading();
			nav.turnTo(76.47);
			nav.travelTo(-5,0);
			while(true){
				RConsole.println("Light value = "+ls.getLightValue());
				try{
					Thread.sleep(1000);
				}catch(InterruptedException e){
					
				}
			}
		} else if (buttonChoice == Button.ID_RIGHT) {
			//The statements in this block will first cause
			//the robot to localise and then proceed to find the light source.
			UltrasonicSensor us = new UltrasonicSensor(SensorPort.S1);
			LightSensor ls = new LightSensor(SensorPort.S2);
			TwoWheeledRobot patBot = new TwoWheeledRobot(Motor.A, Motor.B);
			Odometer odo = new Odometer(patBot, true);
			LCDInfo lcd = new LCDInfo(odo);
			USLocalizer usl = new USLocalizer(odo, us,
					USLocalizer.LocalizationType.FALLING_EDGE);
			usl.doLocalization();
			LightFinder lf = new LightFinder(odo, us, ls);
			lf.locateLightSourceAndGoToIt();
			patBot.setForwardSpeed(0.0);
			patBot.setRotationSpeed(0.0);
		}

		// Wait for another button press to exit
		Button.waitForAnyPress();
		System.exit(0);
	}
}
