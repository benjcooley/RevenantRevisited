// List program memory blocks and basic info.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.mem.MemoryBlock;

public class Sections extends GhidraScript {
  @Override public void run() throws Exception {
    long total = currentProgram.getFunctionManager().getFunctionCount();
    println("functions: " + total);
    for (MemoryBlock b : currentProgram.getMemory().getBlocks()) {
      println(String.format("%s  %s..%s  size=%d  exec=%b init=%b",
        b.getName(), b.getStart(), b.getEnd(), b.getSize(), b.isExecute(), b.isInitialized()));
    }
  }
}
