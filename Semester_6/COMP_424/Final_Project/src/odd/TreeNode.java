package odd;

import java.util.ArrayList;
import java.util.List;

public class TreeNode implements Comparable<TreeNode>{
	private int score;
	private List<TreeNode> children;
	private OddMove oddMove;
	private TreeNode parent;

	public TreeNode() {
		this.score = 0;
		this.children = new ArrayList<TreeNode>();
	}

	public int getScore() {
		return this.score;
	}

	public void setScore(int score) {
		this.score = score;
	}

	public List<TreeNode> getChildren() {
		return this.children;
	}

	public OddMove getOddMove() {
		return this.oddMove;
	}

	public void setOddMove(OddMove oddMove) {
		this.oddMove = oddMove;
	}

	public TreeNode getParent() {
		return this.parent;
	}

	public void setParent(TreeNode parent) {
		this.parent = parent;
	}

	@Override
	public int compareTo(TreeNode treeNode) {
		if(this.score-treeNode.score<0){
			return -1;
		}else if(this.score>0){
			return 1;
		}
		return 0;
	}
}