import lejos.nxt.NXTRegulatedMotor;
import lejos.nxt.*;

public class BangBangController implements UltrasonicController{
	private final int bandCenter, bandwith;
	private final int motorLow, motorHigh;
	private final int motorStraight = 200,FILTER_OUT = 20;
	private final NXTRegulatedMotor leftMotor = Motor.A, rightMotor = Motor.C;
	private int distance;
	private int currentLeftSpeed;
	private int filterControl=0;
	
	public BangBangController(int bandCenter, int bandwith, int motorLow, int motorHigh) {
		//Default Constructor
		this.bandCenter = bandCenter;
		this.bandwith = bandwith;
		this.motorLow = motorLow;
		this.motorHigh = motorHigh;
		leftMotor.setSpeed(motorStraight);
		rightMotor.setSpeed(motorStraight);
		leftMotor.forward();
		rightMotor.forward();
		currentLeftSpeed = 0;
	}
	
	@Override
	public void processUSData(int distance) {
		//this.distance = distance;
		// rudimentary filter to detect gaps and filter out spurious 255 values.
		if (distance >= 50 && filterControl < FILTER_OUT) {
			// bad value, do not set the distance var, however do increment the
			// filter value
			filterControl++;
		} else if (distance >= 50) {
			// true >=50, therefore set distance to the reported distance
			this.distance = distance;
		} else {
			// distance went below 50, therefore reset everything.
			filterControl = 0;
			this.distance = distance;
		}
		// TODO: process a movement based on the us distance passed in (BANG-BANG style)
		//The difference between the ideal distance and the measured distance.
		int error = this.distance - this.bandCenter;
		
		if(Math.abs(error)<=this.bandwith){
			//If the distance reported is equal to the bandCenter +/- the bandwidth, the robot should proceed straight.
			leftMotor.setSpeed(this.motorStraight);
			rightMotor.setSpeed(this.motorStraight);
			leftMotor.forward();
			rightMotor.forward();
		}else if(error<0){
			//If the robot is too close, we increase the speed of the inner wheel and decrease the speed of the outer wheel so that
			//the robot can go back to being an "ideal" distance from the wall.
			leftMotor.setSpeed(this.motorHigh);
			rightMotor.setSpeed(this.motorLow);
			leftMotor.forward();
			rightMotor.forward();
		}else if(error>0){
			//If the robot is too far, we decrease the speed of the inner wheel and increase the speed of the outer wheel so that
			//the robot can go back to being an "ideal" distance from the wall.
			leftMotor.setSpeed(this.motorLow);
			rightMotor.setSpeed(this.motorHigh);
			leftMotor.forward();
			rightMotor.forward();
		}
	}

	@Override
	public int readUSDistance() {
		return this.distance;
	}
}
