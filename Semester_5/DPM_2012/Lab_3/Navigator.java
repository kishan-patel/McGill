import lejos.nxt.LCD;
import lejos.nxt.NXTRegulatedMotor;
import lejos.nxt.SensorPort;
import lejos.nxt.UltrasonicSensor;

public class Navigator extends Thread {
	/**Constant which represents navigating without an obstacle.*/
	public static final int NAVIGATE_WO_OBSTACLE = 1;
	/**Constant which represents navigating with an obstacle.*/
	public static final int NAVIGATE_W_OBSTACLE = 2;
	/**The speed of the motors when traveling forward.*/
	public static final int FORWARD_SPEED = 175; 
	/**The speed of the motors when rotating.*/
	public static final int ROTATION_SPEED = 100;
	/**The treshhold represents the error that is allowed between the 
	 * odometer readings and the fixed coordinates.*/
	public static final int TRESHOLD = 2; 
	/**This parameter is used to control how frequently the heading 
	 * is adjusted when navigating with an obstacle.*/
	public static final int UPDATEHEADINGPERIOD = 4000; 
	/**The default navigation type is set to one without obstacles.*/
	private int typeOfNavigation = 1; 
	/**The odometer is used to get the values of x, y, and theta.*/
	private Odometer odometer; 
	/**Left-motor.*/
	private NXTRegulatedMotor leftMotor;
	/**Right-motor.*/
	private NXTRegulatedMotor rightMotor; 
	/**The array contains all of the coordinates that must be traversed in the path.*/
	private Coordinates[] path; 
	/**The ultrasonic sensor is used to by the bang-bang controller.*/
	private UltrasonicSensor usSensor = new UltrasonicSensor(SensorPort.S1); 
	
	public Navigator(Odometer odometer, NXTRegulatedMotor leftMotor,
						NXTRegulatedMotor rightMotor, Coordinates[] path, int typeOfNavigation) {
		this.odometer = odometer;
		this.leftMotor = leftMotor;
		this.rightMotor = rightMotor;
		this.path = path;
		this.typeOfNavigation = typeOfNavigation;
	}
	
	/**
	 * The run method basically goes through all of the coordinates that must be
	 * traversed in the path and calls either the travelTo or
	 * travelToAvoidingObstacle methods. They both perform a similar task except
	 * the latter method has additional logic to accommodate for the obstacle.
	 */
	@Override
	public void run(){
		if(this.typeOfNavigation == NAVIGATE_WO_OBSTACLE){
			for(Coordinates c: this.path){
				travelTo(c.getX(),c.getY());
			}
			leftMotor.stop();
			rightMotor.stop();
		}else{
			for (Coordinates c : this.path) {
				travelToAvoidingObstacle(c.getX(),c.getY());
			}
			leftMotor.stop();
			rightMotor.stop();
		}
	}
	
	/**
	 * This method is called when the navigation is done without an obstacle. It
	 * is called each time before attempting to reach a give pair of coordinates. It first
	 * updates the heading and then sets the speed of both the motors to
	 * FWDSPEED so that the robot travel straight.
	 * 
	 * @param x
	 *            Represents the destination x coordinate.
	 * @param y
	 *            Represents the destination y coordinate.
	 */
	public void travelTo(double x, double y){
		//For debugging purposes.
		LCD.drawString("dest-x: "+x,0,5);
		LCD.drawString("dest-y: "+y,0,6);
		
		// Apply the angular correction
		double angleToTurn = getCorrectionAngle(x, y);
		turnTo(angleToTurn);	
		
		//Once the heading has been updated, the speed of both the motors is set to the forward
		//speed so that the robot travels straight.
		leftMotor.setSpeed(FORWARD_SPEED);
		rightMotor.setSpeed(FORWARD_SPEED);
		leftMotor.forward();
		rightMotor.forward();

		// This loop executes until the the coordinates reported by the odometer are not
		// equal to the fixed destination coordinate within a certain threshold.
		while (((x - this.odometer.getX() > TRESHOLD || x - this.odometer.getX() < -TRESHOLD))
				|| ((y - this.odometer.getY() > TRESHOLD || y- this.odometer.getY() < -TRESHOLD)));
	}
	
	/**
	 * This method is called when the navigation is done with an obstacle. It
	 * contains the bang-bang controller logic. It is called each time before
	 * attempting to reach a particular pair of coordinates. It periodically
	 * updates the heading, attempts to avoid obstacles, and travels straight.
	 * 
	 * @param x
	 *            Represents the destination x coordinate.
	 * @param y
	 *            Represents the destination y coordinate.
	 */
	public void travelToAvoidingObstacle(double x, double y){
		long start,end; //Use to keep track of when to apply the correction.
		
		//For debuging purposes.
		LCD.drawString("dest-x: "+x,0,4);
		LCD.drawString("dest-y: "+y,0,5);
		
		//Apply the angular correction
		double angleToTurn = getCorrectionAngle(x,y);
		turnTo(angleToTurn);
		
		//Use to keep track of when to apply the correction.
		start = System.currentTimeMillis();
		
		// This loop executes until the the coordinates reported by the odometer are not
		// equal to the fixed destination coordinate within a certain threshold.
		while(((x-this.odometer.getX()>TRESHOLD||x-this.odometer.getX()<-TRESHOLD))
				||((y-this.odometer.getY()>TRESHOLD||y-this.odometer.getY()<-TRESHOLD))){
			//The distance returned by the US sensor.
			int distance = this.usSensor.getDistance();
			
			//For debugging purposes.
			LCD.drawString("Diff: "+(x-this.odometer.getX()), 0, 6);
			
			//Ideally, the robot should be kept 40 cm from the obstacle.
			int error = distance - 40;
			
			if (error < 0) {// The robot is too close to the obstacle. The
							// speeds of the motors should be adjusted.
				leftMotor.setSpeed(350);
				rightMotor.setSpeed(100);
				leftMotor.forward();
				rightMotor.forward();
				start = System.currentTimeMillis();
			} else {// The robot is a correct distance from the obstacles. The
					// speed of the motor should be set to the forward speed.
				leftMotor.setSpeed(175);
				rightMotor.setSpeed(175);
				leftMotor.forward();
				rightMotor.forward();
			}
			
			//Used to keep track of when to apply the correction.
			end = System.currentTimeMillis();
			
			try{
				// If the last time the heading was updated is greater than the
				// BANGBANGPERIOD, the heading should be readjusted again.
				if (end - start > UPDATEHEADINGPERIOD) {
					angleToTurn = getCorrectionAngle(x, y);
					turnTo(angleToTurn);
					start = System.currentTimeMillis();
				}else{
					Thread.sleep(10);
				}
			}catch(InterruptedException e){
				
			}
		}
	}
	
	/**
	 * Turns on point by an amount specified by theta.
	 * 
	 * @param theta
	 *            The angle to turn by.
	 */
	public void turnTo(double theta){
		leftMotor.setSpeed(ROTATION_SPEED);
		rightMotor.setSpeed(ROTATION_SPEED);
		leftMotor.rotate(convertAngle(RobotInfo.leftRadius, RobotInfo.width, theta), true);
		rightMotor.rotate(-convertAngle(RobotInfo.leftRadius, RobotInfo.width, theta), false);
	}
	
	/**
	 * 
	 * @return Indicates whether the robot is navigating.
	 */
	public boolean isNavigating(){
		return false;
	}

	/**
	 * Determines the angle which the robot needs to rotate by to reach the
	 * destination.
	 * 
	 * @param xFinal
	 *            Final x destination coordinate.
	 * @param yFinal
	 *            Final y destination coordinate.
	 * @return The correction angle.
	 */
	private double getCorrectionAngle(double xFinal,double yFinal){
		double x = xFinal - this.odometer.getX();
		double y = yFinal - this.odometer.getY();
		double odometerTheta = this.odometer.getTheta()*180/Math.PI;
		double angleToTurn = 0.0;
		
		if(x>0){
			angleToTurn = 180/Math.PI*Math.atan(y/x);
		}else if(x<0&&y>0){
			angleToTurn = 180/Math.PI*(Math.atan(y/x)+Math.PI);
		}else if(x<0&&y<0){
			angleToTurn = 180/Math.PI*(Math.atan(y/x)-Math.PI);
		}
		
		if((angleToTurn-odometerTheta)<-180){
			angleToTurn = (angleToTurn-odometerTheta)+360;
		}else if((angleToTurn-odometerTheta)>180){
			angleToTurn = (angleToTurn-odometerTheta)-360;
		}else{
			angleToTurn = (angleToTurn-odometerTheta);
		}
		
		return angleToTurn;
	}
	
	/**
	 * Converts distance to the number of degrees to rotate to achieve that
	 * distance.
	 * 
	 * @param radius The radius of the wheels.
	 * @param distance The distance that is to be traveled.
	 * @return The number of degrees to rotate the wheels.
	 */
	private  int convertDistance(double radius, double distance) {
		return (int) ((180.0 * distance) / (Math.PI * radius));
	}

	/**
	 * Converts the number of degrees to rotate by to achieve rotating the robot
	 * at a given angle.
	 * 
	 * @param radius The radius of the wheels.
	 * @param width The width of the robot.
	 * @param angle The angle to turn the robot by.
	 * @return The number of degrees to rotate the wheels.
	 */
	private  int convertAngle(double radius, double width, double angle) {
		return convertDistance(radius, Math.PI * width * angle / 360.0);
	}
	
}
