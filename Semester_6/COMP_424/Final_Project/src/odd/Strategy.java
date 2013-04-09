package odd;

import java.util.LinkedList;

public class Strategy {
	private SearchTree searchTree;
	
	public Strategy(){
		this.searchTree = new SearchTree();
	}
	
	public OddMove getBestMove(OddBoard oddBoard, LinkedList<OddMove> validMoves){
		return bestMoveUsingMCTS(oddBoard,validMoves);
	}
	
	private OddMove bestMoveUsingMCTS(OddBoard oddBoard, LinkedList<OddMove> validMoves){
		TreeNode bestNode = null;
		TreeNode tmpNode = null;
		
		
		for(OddMove oddMove: validMoves){
			if(bestNode == null){
				bestNode = performSimulation(oddBoard,oddMove);
			}else{
				tmpNode = performSimulation(oddBoard,oddMove);
				bestNode = tmpNode.getScore()>bestNode.getScore() ? tmpNode : bestNode;
			}
		}
		
		return bestNode.getOddMove();
	}
	
	private TreeNode performSimulation(OddBoard oddBoard, OddMove oddMove){
		return null;
	}
}
