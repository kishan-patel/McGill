package GridProblem;

public class GridState implements State {

    private boolean wall = false;
    int i, j;
    
    public GridState (int x, int y) {
	i = x; j = y;
    }
    
    public void setWall() {
	wall = true;
    }

    public void clearWall() {
	wall = false;
    }

    public boolean isWall() {
	return wall;
    }
    
    public boolean equals(GridState s) {
	return (i==s.i && j==s.j);
    }

    @Override public void print() {
	System.out.println(i + " " + j);
    }
}