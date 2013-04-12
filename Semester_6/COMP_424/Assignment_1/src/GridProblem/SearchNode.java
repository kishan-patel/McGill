package GridProblem;
public class SearchNode implements Comparable<SearchNode> {
	private int xPos;
	private int yPos;
	private double gScore;
	private double hScore;
	private double fscore;
	private SearchNode parent = null;
	private State state = null;
	private Operator op = null;

	@Override
	public int compareTo(SearchNode o) {
		if(this.fscore-o.fscore<0){
			return -1;
		}else if(this.fscore>0){
			return 1;
		}
		return 0;
	}
	
	public SearchNode(State state, SearchNode parent){
		this.xPos = ((GridState)state).i;
		this.yPos = ((GridState)state).j;
		this.parent = parent;
		this.state = state;
		if(!(this.xPos==1 && this.yPos==1)){
			this.gScore = Double.MAX_VALUE;
		}else{
			this.gScore = 0;
		}
	}

	public int getxPos() {
		return xPos;
	}

	public void setxPos(int xPos) {
		this.xPos = xPos;
	}

	public int getyPos() {
		return yPos;
	}

	public void setyPos(int yPos) {
		this.yPos = yPos;
	}

	public double getgScore() {
		return gScore;
	}

	public void setgScore(double gScore) {
		this.gScore = gScore;
	}

	public double gethScore() {
		return hScore;
	}

	public void sethScore(double hScore) {
		this.hScore = hScore;
	}

	public double getFscore() {
		return fscore;
	}

	public void setFscore(double fscore) {
		this.fscore = fscore;
	}

	public SearchNode getParent() {
		return parent;
	}

	public void setParent(SearchNode parent) {
		this.parent = parent;
	}
	
	public State getGridState(){
		return this.state;
	}
	
	public void setOperator(Operator op){
		this.op = op;
	}
	 
	public Operator getOperator(){
		return this.op;
	}
}
