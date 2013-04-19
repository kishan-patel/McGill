package odd;

import java.util.ArrayList;
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
	private int totalRuns=0;
	private OddBoard gameOddBoard;
	
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
			if(node.getOddBoard().countEmptyPositions()==0){
				return;
			}
			addChildren(node.getOddBoard(),node);
			randomChild = getRandomChild(node);
			/*if(!randomChild.isExpanded()){
				oddBoard.move(randomChild.getOddMove());
				randomChild.setExpanded(true);
			}*/
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
	    		System.out.println("Highest node score = "+(nodeScore+randomizer));
	    		System.out.println("Child visits = "+child.getNoVisits());
	    		System.out.println("Child score = "+child.getScore());
	    		max = nodeScore + randomizer;
	    		bestNodeIndex = i;
         }
      }
	      child = children.get(bestNodeIndex);
	      OddBoard cBoard = (OddBoard)currNode.getOddBoard().clone();
	      cBoard.move(child.getOddMove());
	      child.setBoardState(cBoard);
	      currNode = child;
	      currNode.setParent(null);
	      return currNode.getOddMove();
	}
	
	public MCTSNode getCurrentStateNode(){
		return currNode;
	}
	
	public void determineCurrentStateNode(OddBoard oddBoard){
		List<MCTSNode>currNodeChildren;
		Piece[][] data;
		Piece prevPiece;
		boolean found = false;
		totalRuns = 0;
		/*if(currNode.isLeaf()){
			addChildren(oddBoard,currNode);
		}*/
		gameOddBoard = oddBoard;
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
					removeInvalidChildren(currNode, currNode.getOddBoard().getBoardData());
					found = true;
					break;
				}else{
					data[child.getOddMove().destRow+4][child.getOddMove().destCol+4] = prevPiece;
				}
			}
		}
		
		if(!found){
			System.out.println("Actual");
			System.out.println(currNode.getOddBoard());
			System.out.println("\n\nExpected");
			System.out.println(oddBoard);
		}
	}
	
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
	
	private MCTSNode getRandomChild(MCTSNode node){
		if(node.getChildren().size() == 0){
			return null;
		}
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
		turn = oddBoard.getTurn();
		int emptyPositions = clonedBoard.countEmptyPositions();
		
		//Run simulation until the game is over. At each step, greedily pick 
		//a move that will result in a win for the given player.
		while(emptyPositions!=0){
			//turn = clonedBoard.getTurn();
			validMoves = clonedBoard.getValidMoves();
			/*for(OddMove oddMove: validMoves){
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
				}
			}
			if(!moveMade){
				clonedData[move.destRow + OddBoard.SIZE][move.destCol + OddBoard.SIZE] = move.color;
				turn = (turn == 1)?2:1;
				emptyPositions--;
				clonedBoard.determineWinner();
			}*/
			move = validMoves.get(randomGen.nextInt(validMoves.size()));
			clonedBoard.move(move);
			emptyPositions = clonedBoard.countEmptyPositions();
		}
		clonedBoard.determineWinner();
		//Set the game outcome and return it.
		if(clonedBoard.getWinner() == myTurn){
			 gameOutcome = WIN;
		}else{
			gameOutcome = LOSE;
		}
		totalRuns++;
		return gameOutcome;
	}
	
	private void updateParentScores(MCTSNode node,int gameOutcome){
		MCTSNode parent = node.getParent();
		while(parent != null){
			parent.incrementVisits();
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
		float max = -Float.MAX_VALUE;
		
		//Find the index of the best node.
		for(int i = 0; i<children.size(); i++){
			childNode = children.get(i);
			/*childScore = (float)childNode.getScore()/((float)(childNode.getNoVisits() + Float.MIN_VALUE));
			bias = 2 * (float) (Math.sqrt(Math.log((float) node.getNoVisits()) / ((float) childNode.getNoVisits() + Float.MIN_VALUE)));
			randomizer = Float.MIN_VALUE * randomGen.nextInt(children.size() * children.size());
			biasedScore = childScore + randomizer + (bias * turn);*/
			if(!childNode.isExpanded()){
				childNode.setExpanded(true);
				return childNode;
			}
			
			childScore = (float) (childNode.getScore()/(childNode.getNoVisits()) + Math.sqrt(2*Math.log(totalRuns)/(5*childNode.getNoVisits())));
			if(childScore > max){
				max = childScore;
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
	
	private void removeInvalidChildren(MCTSNode node, Piece[][]latestBoard){
		List<MCTSNode>children = node.getChildren();
		OddMove oddMove;
		List<MCTSNode>nodesToRemove = new ArrayList<MCTSNode>();
		/*for(int i = 0; i<children.size(); i++){
			oddMove = children.get(i).getOddMove();
			if(latestBoard[oddMove.destRow + OddBoard.SIZE][oddMove.destCol + OddBoard.SIZE] != Piece.EMPTY){
				children.remove(i);
			}
		}*/
		for(MCTSNode child: children){
			oddMove = child.getOddMove();
			if(latestBoard[oddMove.destRow + OddBoard.SIZE][oddMove.destCol + OddBoard.SIZE] != Piece.EMPTY){
				nodesToRemove.add(child);
			}
		}
		
		for(MCTSNode child: nodesToRemove){
			children.remove(child);
		}
	}
	
}
