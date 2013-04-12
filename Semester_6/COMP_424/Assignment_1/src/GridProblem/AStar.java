package GridProblem;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.PriorityQueue;
import java.util.Scanner;



public class AStar {
	private static PriorityQueue<SearchNode> open = new PriorityQueue<SearchNode>();	
	private static ArrayList<SearchNode> closed = new ArrayList<SearchNode>();
	private static GridProblem g;
	private static int nn,mm,maxCostRange,heuristicChoice;
	private static double p;
	private static Scanner keyboard = new Scanner(System.in);
	private static State startState, goalState, crntState, nxtState;
	private static SearchNode startNode, goalNode,crntNode, nxtNode;
	private static ArrayList<Operator> a;
	private static HashMap<State,SearchNode> stateToNode = new HashMap<State,SearchNode>();
	private static Operator op;
	private static double tmpGScore;
	
	public static void main(String[] args) throws Exception{			
			//Creates the grid and prints it.
			System.out.println("Enter the nn value");
			nn = keyboard.nextInt();
			System.out.println("Enter the mm value");
			mm = keyboard.nextInt();
			System.out.println("Enter the max cost range value");
			maxCostRange = keyboard.nextInt();
			System.out.println("Enter the p value");
			p = keyboard.nextDouble();
		
			//Ask the user which heuristic function they want to use. 
			do{
				System.out.println("\nMake a choice");
				System.out.println("-------------");
				System.out.println("1-Heuristic Function (h1) = Straight-line distance");
				System.out.println("2-Heuristic Function (h2) = Sum of displacement in x and y axes");
				System.out.println("3-Heuristic Function (h3) = 0.5h1+0.5h2");
				heuristicChoice=keyboard.nextInt();
			}while(heuristicChoice!=1&&heuristicChoice!=2&&heuristicChoice!=3);
			
			//Print the random maze generated.
			System.out.print("\nRandom Maze");
			System.out.println("\n-----------");
			g = new GridProblem(nn,mm,maxCostRange,p);
			g.print();

			// Set the start and goal states.
			startState = g.startState;
			goalState = g.goalState;
			startNode = new SearchNode(startState, null);
			goalNode = new SearchNode(goalState, null);
			startNode.setFscore(0 + getHCost(heuristicChoice, startNode));
			open.offer(startNode);
			stateToNode.put(startState, startNode);

			//The while loop executes until we haven't reach the goal state and
			//there are still search nodes in the priority queue.
			while (!open.isEmpty()) {
				//Get the first element from the priority queue.
				crntNode = open.peek();
				
				// We've reached the destination. We print the path leading to the goal node
				//and return.
				if (g.isGoal(crntNode.getGridState())) {
					System.out.println("\nOutput");
					System.out.println("------");
					constructPath(crntNode);
					return;
				}
				
				//Get the node that is visited next. The neighbors (those that can be reached with legal
				//operations) of this node will be added to the queue as well.
				crntNode = open.poll();
				crntState = crntNode.getGridState();
				closed.add(crntNode);
				a = g.getLegalOps(crntNode.getGridState());
				
				//Goes through all of the neighbors of the current node. The neighbors get added to the queue
				//if they may potentially be on the optimal path to the goal node.
				for (int i = 0; i < a.size(); i++) {
					//Gets an operation to perform on the current state and gets the next state
					//after this operation is performed.
					op = a.get(i);
					nxtState = g.nextState((State) crntState, op);
					
					//If the next state returned is equal to the current state, then it must be a wall
					//so we ignore this operation.
					if (nxtState == crntState) {
						continue;
					}
					
					//Check to see whether a search node has already been created for the current state.
					nxtNode = stateToNode.get(nxtState);
					if (nxtNode == null) {
						nxtNode = new SearchNode(nxtState, crntNode);
						stateToNode.put(nxtState, nxtNode);
					}

					//Compute a temporary gScore for the neighbor.
					tmpGScore = crntNode.getgScore() + g.cost(crntState, op);
				
					//Check to see whether the gScore for the neighbor is greater than
					//the gScore calculated in the above operation. If it is, any search nodes
					//present in the priority queue or list will have to be removed as it may
					//potentially be on the optimal path.
					if (tmpGScore < nxtNode.getgScore()) {
						if (open.contains(nxtNode)) {
							open.remove(nxtNode); // The given node can be reached from a more optimal path.
						}
						if (closed.contains(nxtNode)) {
							closed.remove(nxtNode); // A node already considered can be reached from a more optimal path.
						}
					}

					//Adjust/set the cost of the neighbor and add it to the priority queue.
					if (!open.contains(nxtNode) && !closed.contains(nxtNode)) {
						nxtNode.setParent(crntNode); //Set the parent for the given node.
						nxtNode.setOperator(op);
						nxtNode.setgScore(tmpGScore); // Set the new cost to get to this node from the start node.
						nxtNode.setFscore(nxtNode.getgScore() + getHCost(heuristicChoice, nxtNode)); // Set the estimated cost to get from this node to the goal node.
						open.offer(nxtNode); // Add the node to the priority queue.
					}
				}
			}
			
			//There are no more search nodes in the priority queue and we haven't reached the goal
			//node yet. Thus, there is no valid path from the start node to the goal node.
			System.out.println("No path exists!");
	}
	
	
	private static double getHCost(int choice,SearchNode crntNode){
		switch(choice){
			case 1:
				//h1: straight line distance heuristic function.
				return Math.sqrt(Math.pow((goalNode.getxPos()-crntNode.getxPos()),2)+Math.pow((goalNode.getyPos()-crntNode.getyPos()),2));
			case 2:
				//h2: the sum of the displacement along the x and y axes heuristic function.
				return Math.abs(goalNode.getxPos()-crntNode.getxPos()) + Math.abs(goalNode.getyPos()-crntNode.getyPos());
			case 3:
				//h3: 0.5h1 + 0.5h2
				return 0.5*(Math.sqrt(Math.pow((goalNode.getxPos()-crntNode.getxPos()),2)+Math.pow((goalNode.getyPos()-crntNode.getyPos()),2)))+0.5*((goalNode.getxPos()-crntNode.getxPos()) + (goalNode.getyPos()-crntNode.getyPos()));
			default:
				return Math.sqrt(Math.pow((goalNode.getxPos()-crntNode.getxPos()),2)+Math.pow((goalNode.getyPos()-crntNode.getyPos()),2));
		}
	}
	
	private static void constructPath(SearchNode searchNode){
		if(searchNode.getParent()==null){//We're at the start node
			System.out.println("(i,j)=("+searchNode.getxPos()+","+searchNode.getyPos()+")");
			System.out.println("F cost: "+searchNode.getFscore());
			System.out.println("G cost: "+searchNode.getgScore());
			return;
		}
		constructPath(searchNode.getParent());
		searchNode.getOperator().print();
		System.out.println();
		System.out.println("(i,j)=("+searchNode.getxPos()+","+searchNode.getyPos()+")");
		System.out.println("F cost: "+searchNode.getFscore());
		System.out.println("G cost: "+searchNode.getgScore());
		if(g.isGoal(searchNode.getGridState())){
			System.out.println("\nFinal cost is: "+searchNode.getgScore());
		}
	}
}
