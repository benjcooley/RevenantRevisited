// Disassemble a window of instructions starting at given address (default 32 ins).
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Instruction;

public class Disasm extends GhidraScript {
  @Override public void run() throws Exception {
    String[] args = getScriptArgs();
    long addr = Long.parseLong(args[0].replace("0x",""), 16);
    int count = args.length > 1 ? Integer.parseInt(args[1]) : 32;
    Address a = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(addr);
    Instruction ins = getInstructionAt(a);
    if (ins == null) ins = getInstructionContaining(a);
    for (int i = 0; i < count && ins != null; i++) {
      println(String.format("%s  %s", ins.getAddress(), ins.toString()));
      ins = ins.getNext();
    }
  }
}
