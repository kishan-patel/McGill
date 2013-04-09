package odd;

import java.util.LinkedList;


import boardgame.Board;
import boardgame.Move;
import boardgame.Player;

public class MyPlayer extends Player{
	private OddBoard oddBoard;
	private LinkedList<OddMove> validMoves;
	private OddMove bestMove;
	private Strategy strategy;
	
	public MyPlayer() {
		super("Kishan");
		strategy = new Strategy();
	}

	@Override
	public Move chooseMove(Board board) {
		oddBoard = (OddBoard)board;
		validMoves = oddBoard.getValidMoves();
		bestMove = strategy.getBestMove(oddBoard,validMoves);
		return bestMove;
	}

	public Board createBoard(){
		return new OddBoard();
	}
}
