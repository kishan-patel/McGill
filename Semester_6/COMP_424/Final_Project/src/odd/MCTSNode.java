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

	/**
	 * Constructor.
	 * @param parent
	 */
	public MCTSNode(MCTSNode parent){
		noVisits = 0;
		score = 0;
		expanded = false;
		isLeaf = true;
		this.parent = parent;
		children = new ArrayList<MCTSNode>();
	}

	/**
	 * Returns the score that is associated with this node.
	 * @return
	 */
	public float getScore() {
		return score;
	}

	/**
	 * Sets the score for the given node.
	 * @param score
	 */
	public void setScore(float score) {
		this.score = score;
	}

	/**
	 * Gets the number of times this node has been visited (i.e. number of times it has been part of a simulation).
	 * @return
	 */
	public int getNoVisits() {
		return noVisits;
	}

	/**
	 * Sets the number of times this node has been visited.
	 * @param noVisits
	 */
	public void setNoVisits(int noVisits) {
		this.noVisits = noVisits;
	}

	/**
	 * Increments the number of visits.
	 */
	public void incrementVisits(){
		noVisits++;
	}
	
	/**
	 * Returns whether a simulation has been run from this node. We want every node to be explored at least once.
	 * @return
	 */
	public boolean isExpanded() {
		return expanded;
	}

	/**
	 * Sets wheter a simulation has been run from this node.
	 * @param expanded
	 */
	public void setExpanded(boolean expanded) {
		this.expanded = expanded;
	}

	/**
	 * Returns whether or not this node has children.
	 * @return
	 */
	public boolean isLeaf(){
		return isLeaf;
	}
	
	/**
	 * Set whether or not this node has children.
	 * @param isLeaf
	 */
	public void setIsLeaf(boolean isLeaf){
		this.isLeaf = isLeaf;
	}
	
	/**
	 * Returns the children associated with this node. Each children represents a valid move that can be made
	 * from a given node.
	 * @return
	 */
	public List<MCTSNode> getChildren(){
		return children;
	}
	
	/**
	 * Returns the parent of the node.
	 * @return
	 */
	public MCTSNode getParent(){
		return parent;
	}
	
	/**
	 * Sets the parent of the node.
	 * @param parent
	 */
	public void setParent(MCTSNode parent){
		this.parent = parent;
	}
	
	/**
	 * Add a child to the list of children for a given node.
	 * @param child
	 */
	public void addChildren(MCTSNode child){
		children.add(child);
	}
	
	/**
	 * Get the oddMove that is associated with this node.
	 * @return
	 */
	public OddMove getOddMove(){
		return oddMove;
	}
	
	/**
	 * Set the oddMove that is associated with this node.
	 * @param oddMove
	 */
	public void setOddMove(OddMove oddMove){
		this.oddMove = oddMove;
	}
	
	/**
	 * Returns the boardConfiguration that is associated with this node.
	 * @return
	 */
	public OddBoard getOddBoard() {
		return boardState;
	}
	
	/**
	 * Set the board configuration that is associated with this node.
	 * @param boardState
	 */
	public void setBoardState(OddBoard boardState) {
		this.boardState = boardState;
	}
	
	/**
	 * Increment the score for this node.
	 */
	public void incrementScore(){
		score++;
	}
	
	/**
	 * Decrement the score for this node.
	 */
	public void decrementScore(){
		score--;
	}
	
	/**
	 * Updates the score for the node. If we win, we increment. Otherwise, we decrement.
	 * @param gameOutcome
	 */
	public void updateScore(int gameOutcome){
		if(gameOutcome == WIN){
			incrementScore();
		}else if (gameOutcome == LOSE){
			decrementScore();
		}
	}
}
