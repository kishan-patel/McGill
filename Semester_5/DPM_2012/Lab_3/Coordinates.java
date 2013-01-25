/**
 * Each instance of this class represents a pair of coordinates on the floor.
 *
 */
public class Coordinates {
	double x; 
	double y; 
	
	public Coordinates(double x, double y){
		this.x = x;
		this.y = y;
	}
	
	public double getX(){
		return this.x;
	}
	
	public double getY(){
		return this.y;
	}
}
