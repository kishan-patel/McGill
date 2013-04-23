package odd;

import boardgame.Board;
import boardgame.Move;
import boardgame.Player;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import odd.OddBoard.Piece;

public class MyPlayer extends Player{
    private final int SIMULATIONS_TO_RUN = 200;
    private MCTS mcts;
    
	public MyPlayer() {
		super("Kishan");
		mcts = new MCTS();
	}

	@Override
	public Move chooseMove(Board board) {
		int simulationsToRun = SIMULATIONS_TO_RUN;

		//Set the my player value.
		mcts.setMyTurn(board.getTurn());
		
		//Get the node which corresponds to the current state.
		mcts.determineCurrentStateNode((OddBoard)board);
		
		//Builds the MCST.
		while(simulationsToRun>0){
			mcts.buildMCST((OddBoard)board,mcts.getCurrentStateNode());
			simulationsToRun--;
		}
		
		//Return the best move.
		return mcts.getBestMove();
	}

	public Board createBoard(){
		return new OddBoard();
	}
}

class MCTS {
	private final int WIN = 1;
	private final int LOSE = -1;
	private MCTSNode root;
	private MCTSNode currNode;
	private Random randomGen;
	private int myTurn;
	private int totalRuns=0;
	private OddBoard gameOddBoard;
	
	/**
	 * Constructor.
	 */
	public MCTS(){
		root = new MCTSNode(null);
		currNode = root;
		randomGen = new Random();
	}
	
	/**
	 * Sets the number that is associated with my turn.
	 * @param myTurn
	 */
	public void setMyTurn(int myTurn){
		this.myTurn = myTurn;
	}
	
	/**
	 * Returns the node associated with the current state (last move that was made).
	 * @return
	 */
	public MCTSNode getCurrentStateNode(){
		return currNode;
	}
	
	/**
	 * This method is used to determine what move the other player made. This is needed because we have 
	 * to know where to continue building the MCST from.
	 * @param oddBoard
	 */
	public void determineCurrentStateNode(OddBoard oddBoard){
		List<MCTSNode>currNodeChildren;
		Piece[][] data;
		Piece prevPiece;
		totalRuns = 0;
		gameOddBoard = oddBoard;
		
		if(currNode.isLeaf() && currNode == root){
			currNode.setBoardState(oddBoard);
			addChildren(oddBoard, currNode);
		}else{
			currNodeChildren = currNode.getChildren();
			data = currNode.getOddBoard().getBoardData();
			
			//To determine the other player's move, we know that they must have selected one of the moves 
			//that are associated with the children of our current state. Thus, we go through each children,
			//to know what the other player did.
			for(MCTSNode child: currNodeChildren){
				//Store the current piece at this position in case we need to revert.
				prevPiece = data[child.getOddMove().destRow+4][child.getOddMove().destCol+4];
				
				//Add to the board, the move that is associated with the given child.
				data[child.getOddMove().destRow+4][child.getOddMove().destCol+4] = child.getOddMove().color;
				
				if(dataIsSame(data,oddBoard.getBoardData())){
					//If the board configurations match the configuration of the board that is passed to us then we
					//know that the other player made the move that is associated with the child. Thus, we set the 
					//current node to the child and continue building the tree from this node.
					currNode = child;
					currNode.setBoardState(oddBoard);
					removeInvalidChildren(currNode, currNode.getOddBoard().getBoardData());
					break;
				}else{
					//This is not the move the other player made so we revert.
					data[child.getOddMove().destRow+4][child.getOddMove().destCol+4] = prevPiece;
				}
			}
		}
	}
	
	/**
	 * Builds the MCST recursively. Starting from a given node, the method goes to the leaf and runs the simulation. After running,
	 * the simulation, it propagates the scores. At each step, it determines the next node to select using the getBestSelection() method
	 * which uses the UCB formula. 
	 * @param oddBoard
	 * @param node
	 */
	public void buildMCST(OddBoard oddBoard, MCTSNode node){
		MCTSNode randomChild;
		MCTSNode bestChild;
		int gameOutcome;
		
		if(node.isLeaf()){
			if(node.getOddBoard().countEmptyPositions()==0){
				return;
			}
			addChildren(node.getOddBoard(),node);
			randomChild = getRandomChild(node);
			gameOutcome = simulateGame(node.getOddBoard(),randomChild);
			randomChild.incrementVisits();
			randomChild.updateScore(gameOutcome);
			updateParentScores(randomChild, gameOutcome);
		}else{
			if(node.getOddBoard().countEmptyPositions()==0){
				return;
			}
			removeInvalidChildren(node,gameOddBoard.getBoardData());
			bestChild = getBestSelection(node,myTurn);
			buildMCST(node.getOddBoard(),bestChild);
		}
	}
	
	/**
	 * Gets the best move (move that will result in the highest chances of winning). The best move is determined,
	 * by the score of each node that were set when the simulation was run.
	 * @return
	 */
	public OddMove getBestMove() {
		List<MCTSNode> children = currNode.getChildren();
		float max = -Float.MAX_VALUE;
		int bestNodeIndex = randomGen.nextInt(children.size());
		float randomizer;
		MCTSNode child;

		for (int i = 0; i < children.size(); i++) {
			child = children.get(i);
			float nodeScore = (float) child.getScore() / ((float) (child.getNoVisits() + Float.MIN_VALUE));
			randomizer = Float.MIN_VALUE * randomGen.nextInt(children.size() * children.size());
			if (nodeScore + randomizer > max) {
				max = nodeScore + randomizer;
				bestNodeIndex = i;
			}
		}
		
		//Update the board of the child node to the current node. And set the current node to the child node
		//because this is where we are moving to.
		child = children.get(bestNodeIndex);
		OddBoard cBoard = (OddBoard) currNode.getOddBoard().clone();
		cBoard.move(child.getOddMove());
		child.setBoardState(cBoard);
		currNode = child;
		currNode.setParent(null);
		
		//Return the odd move that is associated with the child node.
		return currNode.getOddMove();
	}
	
	/**
	 * Adds the children (valid moves) for a given node.
	 * @param oddBoard
	 * @param node
	 */
	private void addChildren(OddBoard oddBoard,MCTSNode node){
		OddBoard tmp; 
		List<OddMove> validMoves = node.getOddBoard().getValidMoves();
		for(OddMove oddMove: validMoves){
			tmp = (OddBoard) node.getOddBoard().clone();
			tmp.move(oddMove);
			MCTSNode child = new MCTSNode(node);
			child.setBoardState(tmp);
			child.setOddMove(oddMove);
			node.addChildren(child);
		}
		node.setIsLeaf(false);
	}
	
	/**
	 * Randomly picks a child and returns it from the list of available children.
	 * @param node
	 * @return
	 */
	private MCTSNode getRandomChild(MCTSNode node){
		if(node.getChildren().size() == 0){
			return null;
		}
		int index = randomGen.nextInt(node.getChildren().size());
		return node.getChildren().get(index);
	}
	
	/**
	 * Runs a game from the current node until the end of the game is reached. At each turn, it picks
	 * a random node from the list of valid nodes.
	 * @param oddBoard
	 * @param node
	 * @return
	 */
	private int simulateGame(OddBoard oddBoard, MCTSNode node){
		OddBoard clonedBoard = (OddBoard)oddBoard.clone();
		List<OddMove> validMoves;
		int  gameOutcome;
		OddMove move = null;
		int emptyPositions = clonedBoard.countEmptyPositions();
		
		//Run simulation until the game is over. At each step, pick a random valid move 
		//and execute it.
		while(emptyPositions!=0){
			validMoves = clonedBoard.getValidMoves();
			move = validMoves.get(randomGen.nextInt(validMoves.size()));
			clonedBoard.move(move);
			emptyPositions = clonedBoard.countEmptyPositions();
		}
		
		//The game is over so we determine the winner and return the results.
		totalRuns++;
		clonedBoard.determineWinner();
		if(clonedBoard.getWinner() == myTurn){
			 gameOutcome = WIN;
		}else{
			gameOutcome = LOSE;
		}
		return gameOutcome;
	}
	
	/**
	 * Based on the game outcome, the parent scores are updated. The number of times, this branch
	 * has been played is also updated.
	 * @param node
	 * @param gameOutcome
	 */
	private void updateParentScores(MCTSNode node,int gameOutcome){
		MCTSNode parent = node.getParent();
		
		while(parent != null){
			parent.incrementVisits();
			parent.updateScore(gameOutcome);
			parent = parent.getParent();
		}
	}
	
	/**
	 * Returns the best child (arm) from which we should run our simulation. It uses the UCB formula
	 * to determine this node.
	 * @param node
	 * @param turn
	 * @return
	 */
	private MCTSNode getBestSelection(MCTSNode node,int turn){
		List<MCTSNode> children = node.getChildren();
		MCTSNode childNode;
		int bestNodeIndex = 0;
		float childScore;
		float max = -Float.MAX_VALUE;
		
		//Find the index of the best node.
		for(int i = 0; i<children.size(); i++){
			childNode = children.get(i);
			if(!childNode.isExpanded()){
				childNode.setExpanded(true);
				return childNode;
			}
			//UCB formula is employed to balance between exploration and exploitation.
			childScore = (float) (childNode.getScore()/(childNode.getNoVisits()) + Math.sqrt(2*Math.log(totalRuns)/(5*childNode.getNoVisits())));
			if(childScore > max){
				max = childScore;
				bestNodeIndex = i;
			}
		}
		
		//Return the node to run the simulation from.
		return children.get(bestNodeIndex);
	}
	
	/**
	 * Checks whether the data (pieces on the board) is the same for the two arrays.
	 * @param data1
	 * @param data2
	 * @return
	 */
	private boolean dataIsSame(Piece[][]data1, Piece[][]data2){
		for(int i=0;i<data1.length;i++){
			for(int j=0;j<data1.length;j++){
				if(data1[i][j]!=data2[i][j]){
					return false;
				}
			}
		}
		return true;
	}
	
	/**
	 * Based on the current state of the game, this method removes children that are no longer represent
	 * a valid move.
	 * @param node
	 * @param latestBoard
	 */
	private void removeInvalidChildren(MCTSNode node, Piece[][]latestBoard){
		List<MCTSNode>children = node.getChildren();
		OddMove oddMove;
		List<MCTSNode>nodesToRemove = new ArrayList<MCTSNode>();
		
		for(MCTSNode child: children){
			oddMove = child.getOddMove();
			if(latestBoard[oddMove.destRow + OddBoard.SIZE][oddMove.destCol + OddBoard.SIZE] != Piece.EMPTY){
				//Store the children to remove in a list.
				nodesToRemove.add(child);
			}
		}
		
		for(MCTSNode child: nodesToRemove){
			//Remove all children that were added in the previous loop.
			children.remove(child);
		}
	}
	
}

class MCTSNode {
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
