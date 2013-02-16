import lejos.nxt.LCD;
import lejos.nxt.LightSensor;
import lejos.nxt.Motor;
import lejos.nxt.SensorPort;

/*
 * Odometer.java
 */

public class Odometer extends Thread {
	// robot position
	private double x, y, theta;
	private final double leftWheelRadius=2.69; //2,754
	private final double rightWheelRadius = 2.73; //2.786
	private final double width = 15.55;

	// odometer update period, in ms
	private static final long ODOMETER_PERIOD = 25;

	// lock object for mutual exclusion
	private Object lock;

	// default constructor
	public Odometer() {
		x = 0.0;
		y = 0.0;
		theta = 0.0;
		lock = new Object();
	}

	// run method (required for Thread)
	public void run() {
		long updateStart, updateEnd;
		double tachoLeft=0,tachoRight=0,centerArcLength=0,deltaTheta=0;
		double oldTachoLeft=0,oldTachoRight=0;

		while (true) {
			updateStart = System.currentTimeMillis();

			//The change in the left tachometer value from timestep n to timestep n+1.
			tachoLeft = ((Motor.A.getTachoCount()*Math.PI)/180.0) - oldTachoLeft;
			
			//The left tachometer value at timestep n.
			oldTachoLeft = ((Motor.A.getTachoCount()*Math.PI)/180.0);
			
			//The change in the right tachometer value from timestep n to timestep n+1.
			tachoRight = ((Motor.B.getTachoCount()*Math.PI)/180.0) - oldTachoRight;
			
			//The right tachometer value at timestep n.
			oldTachoRight = ((Motor.B.getTachoCount()*Math.PI)/180.0);
			
			//The arc length traveled by the center of the robot.
            centerArcLength = ((tachoRight*this.rightWheelRadius)+(tachoLeft*this.leftWheelRadius))/2.0;
            
            //The change in theta. If the robot is travelling straight, this should be 0.
            deltaTheta = ((tachoLeft*this.leftWheelRadius)-(tachoRight*this.rightWheelRadius))/this.width;
            
			// put (some of) your odometer code here
			synchronized (lock) {
				//Update the odometer values for x, y, and theta.
                this.x = this.x+(centerArcLength*Math.cos(this.theta+(deltaTheta/2.0)));
                this.y = this.y+(centerArcLength*Math.sin(this.theta+(deltaTheta/2.0)));
                this.theta = theta+deltaTheta;
			}

			// this ensures that the odometer only runs once every period
			updateEnd = System.currentTimeMillis();
			if (updateEnd - updateStart < ODOMETER_PERIOD) {
				try {
					Thread.sleep(ODOMETER_PERIOD - (updateEnd - updateStart));
				} catch (InterruptedException e) {
					// there is nothing to be done here because it is not
					// expected that the odometer will be interrupted by
					// another thread
				}
			}
		}
	}

	// accessors
	public void getPosition(double[] position, boolean[] update) {
		// ensure that the values don't change while the odometer is running
		synchronized (lock) {
			if (update[0])
				position[0] = x;
			if (update[1])
				position[1] = y;
			if (update[2])
				position[2] = theta;
		}
	}

	public double getX() {
		double result;

		synchronized (lock) {
			result = x;
		}

		return result;
	}

	public double getY() {
		double result;

		synchronized (lock) {
			result = y;
		}

		return result;
	}

	public double getTheta() {
		double result;

		synchronized (lock) {
			result = theta;
		}

		return result;
	}

	// mutators
	public void setPosition(double[] position, boolean[] update) {
		// ensure that the values don't change while the odometer is running
		synchronized (lock) {
			if (update[0])
				x = position[0];
			if (update[1])
				y = position[1];
			if (update[2])
				theta = position[2];
		}
	}

	public void setX(double x) {
		synchronized (lock) {
			this.x = x;
		}
	}

	public void setY(double y) {
		synchronized (lock) {
			this.y = y;
		}
	}

	public void setTheta(double theta) {
		synchronized (lock) {
			this.theta = theta;
		}
	}
	
	private static int convertDistance(double radius, double distance) {
		return (int) ((180.0 * distance) / (Math.PI * radius));
	}
}