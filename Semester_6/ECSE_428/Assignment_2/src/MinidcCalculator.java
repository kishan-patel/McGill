import java.util.EmptyStackException;
import java.util.Iterator;
import java.util.Scanner;
import java.util.Stack;


public class MinidcCalculator {
	Stack<Double> stack = new Stack<Double>();
	
	public void displayUserPrompt() {
		String prompt = "Choose from the following:\n"
				+ "P = Prints the value on the top of the stack\n"
				+ "n = Prints the value on the top of the stack and pops it off\n"
				+ "f = Prints the entire contents of the stack\n"
				+ "+ = Adds the last two numerical values\n"
				+ "- = Subtracts the last two numerical values\n"
				+ "* = Multiplies the last two numerical values\n"
				+ "/ = Divides the last two numerical values\n"
				+ "_ [0-9] = Adds a negative number to the stack\n"
				+ "[0-9] = Adds a positive number to the stack\n"
				+ "Q = Quit\n";
		System.out.print(prompt);
	}

	public void printTopValueWithoutPopping(){
		try{
			double val = stack.peek();
			System.out.print("The topmost value is "+val+"\n");
		}catch(EmptyStackException e){
			System.out.print("There are no values on the stack\n");
		}
	}

	public void printTopValueAndPopIt(){
		try{
			double val = stack.pop();
			System.out.print("The topmost value is "+val+" and it has been popped off the stack\n");
		}catch(EmptyStackException e){
			System.out.print("There are no values on the stack\n");
		}
	}
	
	public void printEntireStackWithoutPopping(){
		try{
			stack.peek();
		}catch(EmptyStackException e){
			System.out.print("There are no values on the stack\n");
			return;
		}
		String contents = "The contents of the stack are: ";
		Iterator<Double> it = stack.iterator();
		Double tmp;
		while(it.hasNext()){
			tmp = (Double) it.next();
			contents += tmp;
			if(it.hasNext()){
				contents += ", ";
			}
		}
		contents+="\n";
		System.out.print(contents);
	}

	public void addTwoValuesAndPushResult(){
		try{
			double val1 = stack.pop();
			double val2 = stack.pop();
			double sum = val1+val2;
			stack.push(sum);
		}catch(EmptyStackException e){
			System.out.print("You need more than two values to perform an addition operation\n");
		}
	}

	public void subtractTwoValuesAndPushResult() {
		try {
			double val1 = stack.pop();
			double val2 = stack.pop();
			double difference = val2 - val1;
			stack.push(difference);
		} catch (EmptyStackException e) {
			System.out.print("You need more than two values to perform a subtract operation\n");
		}
	}

	public void multiplyTwoValuesAndPushResult() {
		try {
			double val1 = stack.pop();
			double val2 = stack.pop();
			double total = val1 * val2;
			stack.push(total);
		} catch (EmptyStackException e) {
			System.out.print("You need more than two values to perform a multiplication operation\n");
		}
	}

	public void divideTwoValuesAndPushResult(){
		try{
			double val1 = stack.pop();
			double val2 = stack.pop();
			if (val1 == 0){
				System.out.print("You cannot divide by 0\n");
				return;
			}
			double total = val2/val1;
			stack.push(total);
		}catch (EmptyStackException e) {
			System.out.print("You need more than two values to perform a division operation\n");
		}
	}

	public static void realMain(Scanner input, MinidcCalculator mCalc){
		boolean done = false;
		while(!done){
			String command = input.nextLine();
			if(command.equals("Q")){
				done = true;
			}else if(command.equals("P")){
				mCalc.printTopValueWithoutPopping();
			}else if(command.equals("n")){
				mCalc.printTopValueAndPopIt();
			}else if(command.equals("f")){
				mCalc.printEntireStackWithoutPopping();
			}else if(command.equals("+")){
				mCalc.addTwoValuesAndPushResult();
			}else if(command.equals("-")){
				mCalc.subtractTwoValuesAndPushResult();
			}else if(command.equals("*")){
				mCalc.multiplyTwoValuesAndPushResult();
			}else if(command.equals("/")){
				mCalc.divideTwoValuesAndPushResult();
			}else{
				try{
					double numToAdd;
					int sign = 1;
					if(command.indexOf('_')==0){
						command = command.substring(1);
						sign = -1;
					}
					numToAdd = sign*Double.parseDouble(command);
					if(Double.isInfinite(numToAdd)){
						System.out.print("Cannot process infinity\n");
					}else{
						mCalc.stack.add(numToAdd);
					}
				}catch(NumberFormatException e){
					System.out.print("The syntax is not correct. Please try again.\n");
				}
			}
		}
		System.out.print("The program is going to terminate\n");
	}
	
	public static void main(String[] args){
		MinidcCalculator mCalc = new MinidcCalculator();
		mCalc.displayUserPrompt();
		realMain(new Scanner(System.in), mCalc);
	}
}

