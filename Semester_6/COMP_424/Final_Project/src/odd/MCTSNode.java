package odd;

import java.util.ArrayList;
import java.util.List;

public class MCTSNode {
	private final int WIN = 1;
	private final int LOSE = -1;
	private float score;
	private int noVisits;
	private boolean expanded;	
	private boolean isLeaf;
	private MCTSNode parent;
	private List<MCTSNode> children;
	private OddMove oddMove;
	private OddBoard boardState;

	public MCTSNode(MCTSNode parent){
		noVisits = 0;
		score = 0;
		expanded = false;
		isLeaf = true;
		this.parent = parent;
		children = new ArrayList<MCTSNode>();
	}

	public float getScore() {
		return score;
	}

	public void setScore(float score) {
		this.score = score;
	}

	public int getNoVisits() {
		return noVisits;
	}

	public void setNoVisits(int noVisits) {
		this.noVisits = noVisits;
	}

	public void incVisits(){
		noVisits++;
	}
	
	public boolean isExpanded() {
		return expanded;
	}

	public void setExpanded(boolean expanded) {
		this.expanded = expanded;
	}

	public boolean isLeaf(){
		return isLeaf;
	}
	
	public void setIsLeaf(boolean isLeaf){
		this.isLeaf = isLeaf;
	}
	
	public List<MCTSNode> getChildren(){
		return children;
	}
	
	public MCTSNode getParent(){
		return parent;
	}
	
	public void setParent(MCTSNode parent){
		this.parent = parent;
	}
	
	public void addChildren(MCTSNode child){
		children.add(child);
	}
	
	public OddMove getOddMove(){
		return oddMove;
	}
	
	public void setOddMove(OddMove oddMove){
		this.oddMove = oddMove;
	}
	
	public OddBoard getOddBoard() {
		return boardState;
	}
	
	public void setBoardState(OddBoard boardState) {
		this.boardState = boardState;
	}
	
	public void incrementScore(){
		score++;
	}
	
	public void decrementScore(){
		score--;
	}
	
	public void updateScore(int gameOutcome){
		if(gameOutcome == WIN){
			incrementScore();
		}else if (gameOutcome == LOSE){
			decrementScore();
		}
	}
}
