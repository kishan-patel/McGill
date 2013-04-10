package odd;

import java.util.LinkedList;


import boardgame.Board;
import boardgame.Move;
import boardgame.Player;

public class MyPlayer extends Player{
    protected static final int DEFAULT_TIMEOUT = 5000;
    private MCTS mcts;
    
	public MyPlayer() {
		super("Kishan");
		mcts = new MCTS();
	}

	@Override
	public Move chooseMove(Board board) {
		final long startTime; 
		final long endTime;
		
		//Get the node which corresponds to the current state.
		mcts.determineCurrentStateNode((OddBoard)board);
		
		//Timeout interval.
		startTime = System.currentTimeMillis();
		endTime = startTime + DEFAULT_TIMEOUT;
		
		//Builds the MCST.
		while(System.currentTimeMillis() < endTime){
			mcts.buildMCST((OddBoard)board,mcts.getCurrentStateNode());
		}
		
		//Return the best move.
		return null;
	}

	public Board createBoard(){
		return new OddBoard();
	}
}
