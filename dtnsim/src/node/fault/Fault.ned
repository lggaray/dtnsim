package src.node.fault;

simple Fault
{
    parameters:
        @display("i=block/control");

        bool enable = default(false);
        int faultSeed = default(0);
        double meanTTF @unit(seconds) = default(0); // If 0 no faults enabled 
        double meanTTR @unit(seconds) = default(0); // If 0 no faults enabled
}
