package odd;

import java.util.List;
import java.util.Random;

public class MCTS {
	private MCTSNode root;
	private MCTSNode currNode;
	private Random randomGen;
	
	public MCTS(){
		root = new MCTSNode(null);
		currNode = root;
		randomGen = new Random();
	}
	
	public void buildMCST(OddBoard oddBoard, MCTSNode node){
		MCTSNode randomChild;
		MCTSNode bestChild;
		int gameOutcome;
		
		if(node.isLeaf()){
			addChildren(oddBoard,node);
			randomChild = getRandomChild(node);
			gameOutcome = simulateGame(randomChild);
			randomChild.updateScore(gameOutcome);
			updateParentScores(randomChild, gameOutcome);
		}else{
			bestChild = getBestSelection(node,oddBoard.getTurn());
			buildMCST(bestChild.getOddBoard(),bestChild);
		}
	}
	
	public MCTSNode getBestMove(){
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
	      return children.get(bestNodeIndex);
	}
	
	public MCTSNode getCurrentStateNode(){
		return currNode;
	}
	
	public void determineCurrentStateNode(OddBoard oddBoard){
		List<MCTSNode>currNodeChildren;
		if(currNode.isLeaf()){
			addChildren(oddBoard,currNode);
		}
		currNodeChildren = currNode.getChildren();
		for(MCTSNode child: currNodeChildren){
			if(child.getOddBoard().getBoardData() == oddBoard.getBoardData()){
				currNode = child;
				break;
			}
		}
	}
	
	private void addChildren(OddBoard oddBoard,MCTSNode node){
		OddBoard tmp; 
		List<OddMove>validMoves = oddBoard.getValidMoves();
		for(OddMove move: validMoves){
			tmp = (OddBoard) currNode.getOddBoard().clone();
			tmp.move(move);
			MCTSNode child = new MCTSNode(currNode);
			child.setBoardState(tmp);
			node.addChildren(child);
		}
		node.setIsLeaf(false);
	}
	
	private MCTSNode getRandomChild(MCTSNode node){
		int index = randomGen.nextInt(node.getChildren().size());
		return node.getChildren().get(index);
	}
	
	private int simulateGame(MCTSNode node){
		int gameOutcome = 0;
		//TODO - Simulate game
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
		List<MCTSNode> children = node.getChildren();
		MCTSNode childNode;
		int bestNodeIndex = 0;
		float childScore,bias,randomizer,biasedScore;
		float max = -Float.MAX_VALUE * turn;
		
		for(int i = 0; i<children.size(); i++){
			childNode = children.get(i);
			childScore = (float)childNode.getScore()/((float)(childNode.getNoVisits() + Float.MIN_VALUE));
	         bias = 2 * (float) (Math.sqrt(Math.log((float) node.getNoVisits()) / ((float) childNode.getNoVisits() + Float.MIN_VALUE)));
	         randomizer = Float.MIN_VALUE * randomGen.nextInt(children.size() * children.size());
	         biasedScore = childScore + randomizer + (bias * turn);
	         if(biasedScore*turn > max * turn){
	        	 max = biasedScore;
	        	 bestNodeIndex = i;
	         }
		}
		
		return children.get(bestNodeIndex);
	}
}
