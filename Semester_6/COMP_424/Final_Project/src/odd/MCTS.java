package odd;

import java.util.List;
import java.util.Random;

public class MCTS {
	private final int WIN = 1;
	private final int LOSE = -1;
	private MCTSNode root;
	private MCTSNode currNode;
	private Random randomGen;
	private int myTurn;
	
	public MCTS(){
		root = new MCTSNode(null);
		currNode = root;
		randomGen = new Random();
	}
	
	public void setMyTurn(int myTurn){
		this.myTurn = myTurn;
	}
	
	public void buildMCST(OddBoard oddBoard, MCTSNode node){
		MCTSNode randomChild;
		MCTSNode bestChild;
		int gameOutcome;
		
		if(node.isLeaf()){
			addChildren(oddBoard,node);
			randomChild = getRandomChild(node);
			/*if(!randomChild.isExpanded()){
				oddBoard.move(randomChild.getOddMove());
				randomChild.setExpanded(true);
			}*/
			randomChild.incrementVisits();
			gameOutcome = simulateGame(oddBoard,randomChild);
			randomChild.updateScore(gameOutcome);
			updateParentScores(randomChild, gameOutcome);
		}else{
			bestChild = getBestSelection(node,oddBoard.getTurn());
			buildMCST(bestChild.getOddBoard(),bestChild);
		}
	}
	
	public OddMove getBestMove(){
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
	      currNode = children.get(bestNodeIndex);
	      return children.get(bestNodeIndex).getOddMove();
	}
	
	public MCTSNode getCurrentStateNode(){
		return currNode;
	}
	
	public void determineCurrentStateNode(OddBoard oddBoard){
		List<MCTSNode>currNodeChildren;
		/*if(currNode.isLeaf()){
			addChildren(oddBoard,currNode);
		}*/
		if(!currNode.isLeaf()){
			currNodeChildren = currNode.getChildren();
			for(MCTSNode child: currNodeChildren){
				if(child.getOddBoard().getBoardData() == oddBoard.getBoardData()){
					currNode = child;
					break;
				}
			}
		}else{
			currNode.setBoardState(oddBoard);
		}
	}
	
	private void addChildren(OddBoard oddBoard,MCTSNode node){
		OddBoard tmp; 
		List<OddMove> validMoves = oddBoard.getValidMoves();
		for(OddMove oddMove: validMoves){
			tmp = (OddBoard) currNode.getOddBoard().clone();
			tmp.move(oddMove);
			MCTSNode child = new MCTSNode(currNode);
			child.setBoardState(tmp);
			child.setOddMove(oddMove);
			node.addChildren(child);
		}
		node.setIsLeaf(false);
	}
	
	private MCTSNode getRandomChild(MCTSNode node){
		int index = randomGen.nextInt(node.getChildren().size());
		return node.getChildren().get(index);
	}
	
	private int simulateGame(OddBoard oddBoard, MCTSNode node){
		OddBoard clonedBoard = (OddBoard)oddBoard.clone();
		OddBoard tmp = (OddBoard)oddBoard.clone();
		List<OddMove> validMoves;
		int turn, gameOutcome;
		boolean moveMade = false;
		OddMove move = null;
		
		//Run simulation until the game is over. At each step, greedily pick 
		//a move that will result in a win for the given player.
		while(clonedBoard.countEmptyPositions()!=0){
			turn = clonedBoard.getTurn();
			validMoves = clonedBoard.getValidMoves();
			for(OddMove oddMove: validMoves){
				moveMade = false;
				move = oddMove;
				tmp.move(oddMove);
				tmp.determineWinner();
				if(tmp.getWinner() == turn){
					clonedBoard = (OddBoard)tmp.clone();
					moveMade = true;
					break;
				}else{
					tmp = (OddBoard)clonedBoard.clone();
				}
			}
			if(!moveMade){
				tmp.move(move);
				clonedBoard = (OddBoard)tmp.clone();
			}
		}
		
		//Set the game outcome and return it.
		if(clonedBoard.getWinner() == myTurn){
			 gameOutcome = WIN;
		}else{
			gameOutcome = LOSE;
		}
		return gameOutcome;
	}
	
	private void updateParentScores(MCTSNode node,int gameOutcome){
		MCTSNode parent = node.getParent();
		while(parent != null){
			parent.updateScore(gameOutcome);
			parent = parent.getParent();
		}
	}
	
	private MCTSNode getBestSelection(MCTSNode node,int turn){
		if(turn == myTurn){
			turn = 1;
		}else{
			turn = -1;
		}
		List<MCTSNode> children = node.getChildren();
		MCTSNode childNode;
		int bestNodeIndex = 0;
		float childScore,bias,randomizer,biasedScore;
		float max = -Float.MAX_VALUE * turn;
		
		//Find the index of the best node.
		for(int i = 0; i<children.size(); i++){
			childNode = children.get(i);
			childScore = (float)childNode.getScore()/((float)(childNode.getNoVisits() + Float.MIN_VALUE));
			bias = 2 * (float) (Math.sqrt(Math.log((float) node.getNoVisits()) / ((float) childNode.getNoVisits() + Float.MIN_VALUE)));
			randomizer = Float.MIN_VALUE * randomGen.nextInt(children.size() * children.size());
			biasedScore = childScore + randomizer + (bias * turn);
			if(biasedScore*turn > max*turn){
				max = biasedScore;
				bestNodeIndex = i;
			}
		}
		
		//Return the node to run the simulation from.
		return children.get(bestNodeIndex);
	}
}
