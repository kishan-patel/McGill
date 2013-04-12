

import static org.junit.Assert.assertEquals;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Scanner;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class MinidcInputSyntaxTests {
	private final ByteArrayOutputStream outContent = new ByteArrayOutputStream();
	private final ByteArrayOutputStream errContent = new ByteArrayOutputStream();
	private MinidcCalculator minidcCalc;
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
	public void processInputQFromMainTest(){
		Scanner s = new Scanner("Q\n");
		MinidcCalculator.realMain(s, minidcCalc);
		assertEquals("The program is going to terminate\n", outContent.toString());
	}
	
	@Test
	public void processInputPFromMainTest(){
		Scanner s = new Scanner("P\nQ\n");
		String aString = "The contents of the stack are: 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0\n";
		for(int i=1;i<=10;i++){
			minidcCalc.stack.add(i*1.0);
		}
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The topmost value is 10.0\nThe program is going to terminate\n"+aString, outContent.toString());
	}
	
	@Test
	public void processInputnFromMainTest(){
		Scanner s = new Scanner("n\nQ\n");
		String aString = "The contents of the stack are: 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0\n";
		for(int i=1;i<=10;i++){
			minidcCalc.stack.add(i*1.0);
		}
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The topmost value is 10.0 and it has been popped off the stack\nThe program is going to terminate\n"+aString, outContent.toString());
	}
	
	@Test
	public void processInputfFromMainTest(){
		Scanner s = new Scanner("f\nQ\n");
		String aString = "The contents of the stack are: 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0\n";
		for(int i=1;i<=10;i++){
			minidcCalc.stack.add(i*1.0);
		}
		MinidcCalculator.realMain(s, minidcCalc);
		assertEquals(aString+"The program is going to terminate\n", outContent.toString());
	}
	
	@Test
	public void processInputPlusFromMainTest(){
		Scanner s = new Scanner("+\nQ\n");
		minidcCalc.stack.add(5.0);
		minidcCalc.stack.add(5.0);
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The program is going to terminate\nThe contents of the stack are: 10.0\n", outContent.toString());
	}
	
	@Test
	public void processInputMinusFromMainTest(){
		Scanner s = new Scanner("-\nQ\n");
		minidcCalc.stack.add(5.0);
		minidcCalc.stack.add(5.0);
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The program is going to terminate\nThe contents of the stack are: 0.0\n", outContent.toString());
	}
	
	@Test
	public void processInputTimesFromMainTest(){
		Scanner s = new Scanner("*\nQ\n");
		minidcCalc.stack.add(5.0);
		minidcCalc.stack.add(5.0);
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The program is going to terminate\nThe contents of the stack are: 25.0\n", outContent.toString());
	}
	
	@Test
	public void processInputDivideFromMainTest(){
		Scanner s = new Scanner("/\nQ\n");
		minidcCalc.stack.add(5.0);
		minidcCalc.stack.add(5.0);
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The program is going to terminate\nThe contents of the stack are: 1.0\n", outContent.toString());
	}
	
	
	
	@Test
	public void processPositiveNumFromMainTest(){
		Scanner s = new Scanner("5.\nQ\n");
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The program is going to terminate\nThe contents of the stack are: 5.0\n", outContent.toString());
	}
	
	@Test
	public void processNegativeNumFromMainTest(){
		Scanner s = new Scanner("_5.0\nQ\n");
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The program is going to terminate\nThe contents of the stack are: -5.0\n", outContent.toString());
	}
	
	@Test
	public void processInvalidSyntaxTest() {
			Scanner s = new Scanner("_5.0_\n_.5\nQ\n");
			MinidcCalculator.realMain(s, minidcCalc);
			assertEquals("The syntax is not correct. Please try again.\nThe program is going to terminate\n",outContent.toString());

	}
}
