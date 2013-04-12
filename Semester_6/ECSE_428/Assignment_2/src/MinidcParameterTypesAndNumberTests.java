import static org.junit.Assert.assertEquals;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import java.util.Scanner;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;


public class MinidcParameterTypesAndNumberTests {
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
	public void numberInRangeTest() {
		Scanner s = new Scanner("5.0\n10\nQ\n");
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The program is going to terminate\nThe contents of the stack are: 5.0, 10.0\n", outContent.toString());
	}
	
	@Test 
	public void numberNotInRangeTest(){
		double d = Double.POSITIVE_INFINITY;
		Scanner s = new Scanner(d+"\nQ\n");
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("Cannot process infinity\nThe program is going to terminate\nThere are no values on the stack\n", outContent.toString());
		
	}
	
	@Test 
	public void validParameterTest(){
		Scanner s = new Scanner("5.0\nQ\n");
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The program is going to terminate\nThe contents of the stack are: 5.0\n", outContent.toString());
		
	}
	
	@Test 
	public void invalidParameterTest(){
		Scanner s = new Scanner("*5.0\nQ\n");
		MinidcCalculator.realMain(s, minidcCalc);
		minidcCalc.printEntireStackWithoutPopping();
		assertEquals("The syntax is not correct. Please try again.\nThe program is going to terminate\nThere are no values on the stack\n", outContent.toString());
		
	}

}
