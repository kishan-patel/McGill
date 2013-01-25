import lejos.nxt.LCD;
import lejos.nxt.LightSensor;
import lejos.nxt.SensorPort;
import lejos.nxt.Sound;

/* 
 * OdometryCorrection.java
 */

public class OdometryCorrection extends Thread {
	private static final long CORRECTION_PERIOD = 25;
	private Odometer odometer;
	LightSensor lightSensor = new LightSensor(SensorPort.S1);
	public final int offSet = 12;

	// constructor
	public OdometryCorrection(Odometer odometer) {
		this.odometer = odometer;
	}

	// run method (required for Thread)
	public void run() {
		long correctionStart, correctionEnd;
		double position;
		double adjustedPosition;
		int lightValue;
		int xLineCount = 0;
		int yLineCount = 0;

		while (true) {
			correctionStart = System.currentTimeMillis();

			// put your correction code here
			//The value detected by the light sensor. The lower the value is, the darker the light.
			lightValue = lightSensor.getLightValue();
			
			//For debugging purposes.
			LCD.drawString("L-Value: "+lightValue, 0, 4);
			
			if(lightValue<40){//Black lines had a light value below 40.
				
				//For debugging purposes.
				LCD.drawString("B Detec.", 0, 5);
				Sound.beep();
				
				//The approach taken for the odometer correction is to keep  two counts representing the number of black lines encountered on the x and y axes.
				//If we are on the positive axis and a line is encountered, the line count is incremented otherwise it is decremented.
				//The position of the robot on a given axis is adjusted when a line is encountered. The position is determined by the formula 30.48*lineCount + offset.
				if(this.odometer.getTheta()>=-1 && this.odometer.getTheta()<=1){//Case where the robot is traveling on the positive x axis.
					 LCD.drawString("PLCX: "+xLineCount,0,6);
					 adjustedPosition = (30.48*xLineCount)+12;
					 xLineCount++;
					 this.odometer.setX(adjustedPosition);
				}else if(this.odometer.getTheta()>=1.27 && this.odometer.getTheta()<=1.77){//Case where the robot is traveling on the positive y axis.
					LCD.drawString("PLCY: "+yLineCount,0,6);
					adjustedPosition = (30.48*yLineCount)+12;
					yLineCount++;
					this.odometer.setY(adjustedPosition);
				}else if(this.odometer.getTheta()>=2.94 && this.odometer.getTheta()<=3.34){//Case where the robot is traveling on the negative x axis.
					xLineCount--;
					LCD.drawString("NLCX: "+xLineCount,0,6);
					adjustedPosition = (30.48*xLineCount)+18;
					this.odometer.setX(adjustedPosition);
				}else if(this.odometer.getTheta()>=4.51&&this.odometer.getTheta()<=4.91){//Case where the robot is travelling on the negative y axis.
					yLineCount--;
					LCD.drawString("NLCY: "+yLineCount,0,6);
					adjustedPosition = (30.48*yLineCount)+18;
					this.odometer.setY(adjustedPosition);
				}
				
				//A sleep is employed here to prevent the line counters from incrementing multiple times when a single line is encountered.
				try{
					Thread.sleep(2000);
				}catch(InterruptedException e){
					
				}
			}
			
			// this ensure the odometry correction occurs only once every period
			correctionEnd = System.currentTimeMillis();
			if (correctionEnd - correctionStart < CORRECTION_PERIOD) {
				try {
					Thread.sleep(CORRECTION_PERIOD
							- (correctionEnd - correctionStart));
				} catch (InterruptedException e) {
					// there is nothing to be done here because it is not
					// expected that the odometry correction will be
					// interrupted by another thread
				}
			}
		}
	}
}