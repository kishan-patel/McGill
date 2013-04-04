package GridProblem;

import java.lang.*;
import java.util.*;

public interface Problem { 

    public boolean isLegal (State s, Operator op);
    public ArrayList<Operator> getLegalOps (State s);
    public State nextState (State s, Operator op); 
    public int cost(State s, Operator op);
    public State getStartState();
    public boolean isStart (State s);
    public boolean isGoal (State s);
}