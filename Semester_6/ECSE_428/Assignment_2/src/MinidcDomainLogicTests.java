
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.util.EmptyStackException;
import java.util.Scanner;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class MinidcDomainLogicTests {
	private final ByteArrayOutputStream outContent = new ByteArrayOutputStream();
	private final ByteArrayOutputStream errContent = new ByteArrayOutputStream();
	private MinidcCalculator minidcCalc;

	@Before
	public void setupStreams() {
		System.setOut(new PrintStream(outContent));
		System.setErr(new PrintStream(errContent));
		minidcCalc = new MinidcCalculator();
	}

	@After
	public void cleanupStreams() {
		System.setOut(null);
		System.setErr(null);
		minidcCalc = null;
	}

	@Test
	public void displayUserPromptTest() {
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
				+"Q = Quit\n";
		
		minidcCalc.displayUserPrompt();
		assertEquals(prompt, outContent.toString());
	}
	
	@Test
	public void printTopValueWithoutPoppingTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.printTopValueWithoutPopping();
		minidcCalc.printTopValueWithoutPopping();
		assertEquals("The topmost value is 5.0\nThe topmost value is 5.0\n", outContent.toString());
	}
	
	@Test
	public void printTopValueAndPopItTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.printTopValueAndPopIt();
		assertEquals("The topmost value is 5.0 and it has been popped off the stack\n", outContent.toString());
		assertEquals("The stack should be empty", true,minidcCalc.stack.empty());

	}
	
	
	@Test
	public void printEntireStackWithoutPoppingTest(){
		String aString = "The contents of the stack are: 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0\n";
		for(int i=1;i<=10;i++){
			minidcCalc.stack.add(i*1.0);
		}
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals(aString,outContent.toString());
		try{
			minidcCalc.stack.peek();
		}catch(EmptyStackException e){
			fail("The elements should not have been removed.\n");
		}
	}
	
	@Test 
	public void addTwoValuesAndPushResultTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.stack.add(5.0);
		minidcCalc.addTwoValuesAndPushResult();
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The contents of the stack are: 10.0\n", outContent.toString());
	}
	
	@Test
	public void subtractTwoValuesAndPushResultTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.stack.add(5.0);
		minidcCalc.subtractTwoValuesAndPushResult();
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The contents of the stack are: 0.0\n", outContent.toString());
	}
	
	@Test
	public void multiplyTwoValuesAndPushResultTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.stack.add(5.0);
		minidcCalc.multiplyTwoValuesAndPushResult();
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The contents of the stack are: 25.0\n", outContent.toString());
	}
	
	
	@Test
	public void divideTwoValuesAndPushResultTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.stack.add(5.0);
		minidcCalc.divideTwoValuesAndPushResult();
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The contents of the stack are: 1.0\n", outContent.toString());
	}
	
	
	
	@Test
	public void addOneOrLessValuesTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.addTwoValuesAndPushResult();
		assertEquals("You need more than two values to perform an addition operation\n",outContent.toString());
	}
	
	@Test
	public void subtractOneOrLessValuesTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.subtractTwoValuesAndPushResult();
		assertEquals("You need more than two values to perform a subtract operation\n",outContent.toString());
	}
	
	@Test
	public void multiplyOneOrLessValuesTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.multiplyTwoValuesAndPushResult();
		assertEquals("You need more than two values to perform a multiplication operation\n",outContent.toString());
	}
	
	@Test
	public void divideOneOrLessValuesTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.divideTwoValuesAndPushResult();
		assertEquals("You need more than two values to perform a division operation\n",outContent.toString());
	}
	
	@Test
	public void divideByZeroTest(){
		minidcCalc.stack.add(5.0);
		minidcCalc.stack.add(0.0);
		minidcCalc.divideTwoValuesAndPushResult();
		assertEquals("You cannot divide by 0\n",outContent.toString());
	}
	
	@Test
	public void peekPrintWithNoValuesOnStackTest(){
		minidcCalc.printTopValueWithoutPopping();
		assertEquals("There are no values on the stack\n",outContent.toString());
	}
	
	@Test
	public void popPrintWithNoValuesOnStackTest(){
		minidcCalc.printTopValueAndPopIt();
		assertEquals("There are no values on the stack\n",outContent.toString());
	}
	
	@Test
	public void entirePrintWithNoValuesOnStackTest(){
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("There are no values on the stack\n",outContent.toString());
	}
}
