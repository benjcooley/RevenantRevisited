// Find functions whose name (or namespace::name) contains the substring.
// arg0 = needle, arg1 (optional) = output file path
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import java.io.PrintWriter;

public class FindByName extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    if (args.length < 1) { println("usage: FindByName <substring> [outfile]"); return; }
    String needle = args[0];
    PrintWriter w = args.length > 1 ? new PrintWriter(args[1]) : null;
    int found = 0;
    for (Function fn : currentProgram.getFunctionManager().getFunctions(true)) {
      String full = fn.getName(true); // includes namespace
      if (full.contains(needle)) {
        String line = String.format("%08x  %s",
            fn.getEntryPoint().getOffset(), full);
        println(line);
        if (w != null) w.println(line);
        found++;
      }
    }
    if (w != null) w.close();
    println("matches: " + found);
  }
}
