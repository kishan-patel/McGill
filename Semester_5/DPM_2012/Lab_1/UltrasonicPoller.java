import lejos.nxt.UltrasonicSensor;


public class UltrasonicPoller extends Thread{
	private UltrasonicSensor us;
	private UltrasonicController cont;
	
	public UltrasonicPoller(UltrasonicSensor us, UltrasonicController cont) {
		this.us = us;
		this.cont = cont;
	}
	
	public void run() {
		while (true) {
			//process collected data
			//Note: The filtering of spurious 255 values is applied in the controller.
			cont.processUSData(us.getDistance());
			try { Thread.sleep(10); } catch(Exception e){}
		}
	}

}
