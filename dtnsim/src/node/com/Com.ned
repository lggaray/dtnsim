package src.node.com;

simple Com
{
    parameters:
        @display("i=block/wtx");
        
        double packetLoss = default(0.0);
                      
    gates:
        inout gateToDtn;
        input gateToAir @directIn;
}
