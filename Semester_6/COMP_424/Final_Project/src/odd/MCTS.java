package odd;

import java.util.List;
import java.util.Random;

import odd.OddBoard.Piece;

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
			addChildren(node.getOddBoard(),node);
			randomChild = getRandomChild(node);
			/*if(!randomChild.isExpanded()){
				oddBoard.move(randomChild.getOddMove());
				randomChild.setExpanded(true);
			}*/
			randomChild.incrementVisits();
			gameOutcome = simulateGame(node.getOddBoard(),randomChild);
			randomChild.updateScore(gameOutcome);
			updateParentScores(randomChild, gameOutcome);
		}else{
			bestChild = getBestSelection(node,oddBoard.getTurn());
			buildMCST(node.getOddBoard(),bestChild);
		}
	}
	
	public OddMove getBestMove(){
		List<MCTSNode> children = currNode.getChildren();
		float max = -Float.MAX_VALUE;
	    int bestNodeIndex = randomGen.nextInt(children.size());
	    float randomizer;
	    MCTSNode child;
	    //System.out.println("before");
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
	      for(MCTSNode c: currNode.getChildren()){
		    	//System.out.println("x = " + c.getOddMove().destRow + ". y = "+ c.getOddMove().destCol + ".");
	      }
	      return children.get(bestNodeIndex).getOddMove();
	}
	
	public MCTSNode getCurrentStateNode(){
		return currNode;
	}
	
	public void determineCurrentStateNode(OddBoard oddBoard){
		List<MCTSNode>currNodeChildren;
		Piece[][] data;
		Piece prevPiece;
		boolean found = false;
		/*if(currNode.isLeaf()){
			addChildren(oddBoard,currNode);
		}*/
		if(currNode.isLeaf() && currNode == root){
			currNode.setBoardState(oddBoard);
			addChildren(oddBoard, currNode);
		}else{
			if(currNode.isLeaf()){
				addChildren(currNode.getOddBoard(), currNode);
			}
			//System.out.println("after");
			currNodeChildren = currNode.getChildren();
			data = currNode.getOddBoard().getBoardData();
			for(MCTSNode child: currNodeChildren){
				//System.out.println("x = " + child.getOddMove().destRow + ". y = "+ child.getOddMove().destCol + ".");
				prevPiece = data[child.getOddMove().destRow+4][child.getOddMove().destCol+4];
				data[child.getOddMove().destRow+4][child.getOddMove().destCol+4] = child.getOddMove().color;
				if(dataIsSame(data,oddBoard.getBoardData())){
					currNode = child;
					currNode.setBoardState(oddBoard);
					found = true;
					break;
				}else{
					data[child.getOddMove().destRow+4][child.getOddMove().destCol+4] = prevPiece;
				}
			}
		}
		
		if(currNode != root && !found){
			System.out.println("Actual");
			System.out.println(currNode.getOddBoard());
			System.out.println("\n\nExpected");
			System.out.println(oddBoard);
		}
	}
	
	private void addChildren(OddBoard oddBoard,MCTSNode node){
		OddBoard tmp; 
		List<OddMove> validMoves = currNode.getOddBoard().getValidMoves();
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
		List<OddMove> validMoves;
		int turn, gameOutcome;
		boolean moveMade = false;
		OddMove move = null;
		Piece prevPiece;
		Piece[][] clonedData = clonedBoard.getBoardData();
		turn = clonedBoard.getTurn();
		int emptyPositions = clonedBoard.countEmptyPositions();
		
		//Run simulation until the game is over. At each step, greedily pick 
		//a move that will result in a win for the given player.
		while(emptyPositions!=0){
			//turn = clonedBoard.getTurn();
			validMoves = clonedBoard.getValidMoves();
			for(OddMove oddMove: validMoves){
				moveMade = false;
				move = oddMove;
				prevPiece = clonedData[oddMove.destRow + OddBoard.SIZE][oddMove.destCol + OddBoard.SIZE];
				clonedData[oddMove.destRow + OddBoard.SIZE][oddMove.destCol + OddBoard.SIZE] = oddMove.color;
				clonedBoard.determineWinner();
				if(clonedBoard.getWinner() == turn){
					moveMade = true;
					turn = (turn == 1)?2:1;
					emptyPositions--;
					break;
				}else{
					clonedData[oddMove.destRow + OddBoard.SIZE][oddMove.destCol + OddBoard.SIZE] = prevPiece;
					//System.out.println("undoing");
				}
			}
			if(!moveMade){
				clonedData[move.destRow + OddBoard.SIZE][move.destCol + OddBoard.SIZE] = move.color;
				turn = (turn == 1)?2:1;
				emptyPositions--;
				clonedBoard.determineWinner();
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
	
	
}
