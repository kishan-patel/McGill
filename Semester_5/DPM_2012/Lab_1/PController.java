import lejos.nxt.*;

public class PController implements UltrasonicController {
	
	private final int bandCenter, bandwith;
	private final int motorStraight = 150, FILTER_OUT = 40;
	private final NXTRegulatedMotor leftMotor = Motor.A, rightMotor = Motor.C;	
	private int distance;
	private int currentLeftSpeed;
	private int filterControl;
	public PController(int bandCenter, int bandwith) {
		//Default Constructor
		this.bandCenter = bandCenter;
		this.bandwith = bandwith;
		leftMotor.setSpeed(motorStraight);
		rightMotor.setSpeed(motorStraight);
		leftMotor.forward();
		rightMotor.forward();
		currentLeftSpeed = 0;
		filterControl = 0;
	}
	
	@Override
	public void processUSData(int distance) {
		// rudimentary filter to detect gaps and filter out spurious 255 values.
		if (distance >= 50 && filterControl < FILTER_OUT) {
			// bad value, do not set the distance var, however do increment the filter value
			filterControl ++;
		} else if (distance >= 50){
			// true >=50, therefore set distance to reported distance.
			this.distance = distance;
		} else {
			// distance went below 50, therefore reset everything.
			filterControl = 0;
			this.distance = distance;
		}
		
		//The difference between the ideal distance and the measured distance.
		int error = this.distance - bandCenter;
		int absError = Math.abs(error);
		
		//The error is multiplied by a constant to get the change in speed that is required. 
		//i.e. change in speed = 5 * distance from the ideal distance
		int changeInSpeed = absError*7;
		
		//For debugging purposes.
		LCD.drawString("Error: "+error,0,6);
		
		if(absError<=this.bandwith){
			//If we are the ideal distance from the wall, the speed of the two motors are set to the default speed to go straight.
			leftMotor.setSpeed(this.motorStraight);
			rightMotor.setSpeed(this.motorStraight);
			leftMotor.forward();
			rightMotor.forward();
			
			//For debugging purposes.
			LCD.drawString("Left: "+this.motorStraight,0,7);
			LCD.drawString("Right: "+this.motorStraight,0,8);
		}else if(error<0){
			//If the robot is too close, we increase the speed of the inner wheel and decrease the speed of the outer wheel so that
			//the robot can go back to being an "ideal" distance from the wall. In both cases we either add or subtract 
			//the changeInSpeed (proportional to the error) from the base speed.
			leftMotor.setSpeed(changeInSpeed<=500?this.motorStraight+changeInSpeed:500);
			rightMotor.setSpeed(changeInSpeed<=this.motorStraight?this.motorStraight-changeInSpeed:0);
			leftMotor.forward();
			rightMotor.forward();
			
			//For debugging purposes.
			LCD.drawString("Left: "+(this.motorStraight+changeInSpeed),0,7);
			LCD.drawString("Right: "+(this.motorStraight-changeInSpeed),0,8);
		}else if(error>0){
			//If the robot is too close, we decrease the speed of the inner wheel and increase the speed of the outer wheel so that
			//the robot can go back to being an "ideal" distance from the wall. In both cases we either add or subtract 
			//the changeInSpeed (proportional to the error) from the base speed.
			leftMotor.setSpeed(changeInSpeed<=this.motorStraight?this.motorStraight-changeInSpeed:0);
			rightMotor.setSpeed(changeInSpeed<=500?this.motorStraight+changeInSpeed:500);
			leftMotor.forward();
			rightMotor.forward();
			
			//For debugging purposes.
			LCD.drawString("Left: "+(this.motorStraight-changeInSpeed),0,7);
			LCD.drawString("Right: "+(this.motorStraight+changeInSpeed),0,8);
		}
	}

	
	@Override
	public int readUSDistance() {
		return this.distance;
	}

}
