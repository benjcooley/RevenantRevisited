import ghidra.app.script.GhidraScript;
public class ListPrograms extends GhidraScript {
  @Override public void run() throws Exception {
    println("program: " + currentProgram.getName());
  }
}
