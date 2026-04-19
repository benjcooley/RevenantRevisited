import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.address.Address;

public class FindFn extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        long target = args.length > 0 ? Long.parseLong(args[0].replace("0x",""), 16) : 0x471ce0L;
        Address a = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(target);
        println("target addr: " + a);
        Function c = getFunctionContaining(a);
        println("containing: " + (c==null?"<none>":c.getName()+"@"+c.getEntryPoint()));
        // Scan functions near target
        int count = 0;
        for (Function fn : currentProgram.getFunctionManager().getFunctions(true)) {
            long entry = fn.getEntryPoint().getOffset();
            if (Math.abs(entry - target) < 0x200) {
                println(String.format("  %08x  %s", entry, fn.getName()));
            }
            count++;
        }
        println("total functions: " + count);
    }
}
