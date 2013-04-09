package odd;

import java.util.Collections;
import java.util.List;

public class SearchTree {
	private TreeNode root;

	public TreeNode getRoot(){
		return this.root;
	}
	
	public void setRoot(TreeNode root){
		this.root = root;
	}
	
	public TreeNode findBestNodeToStart(){
		boolean done = false;
		List<TreeNode> children;
		TreeNode leaf = null;
		
		while(!done){
			children = root.getChildren();
			Collections.sort(children);
			if(children.get(0)==null){
				done = true;
			}else{
				leaf = children.get(0);
			}
		}
		return leaf;
	}
}
