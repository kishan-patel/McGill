package GridProblem;

public class GridOperator implements Operator {

    int delta_i, delta_j;
    String name;

    public GridOperator(String s, int x, int y) {
	name = s;
	delta_i = x;
	delta_j = y;
    }

    @Override public void print() {
	System.out.println(name);
    }
}