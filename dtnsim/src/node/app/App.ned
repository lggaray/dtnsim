package src.node.app;

simple App
{
    parameters:
        @display("i=block/telnet");
        
        bool enable = default(false); // Traffic Generator Enabled
        string destinationEid = default("1"); // Generated Bundles destination EiD
        string bundlesNumber = default("1"); // Number of Bundles to Generate
        string size = default("1024"); // Generated Bundles Size in Bytes
        string start = default("0"); // tiempo de generación de mensajes
        double interval = default(0); // Bundles Generation Interval in Secs
        double ttl = default(9000000); // Generated Bundles Time to Live in Secs
        bool returnToSender = default(true);
        bool critical = default(false);
        bool custodyTransfer = default(false);
        string externalTrafficEvents = default("");
        
        // Signals and Statistics
        @signal[appBundleSent];
        @signal[appBundleReceived];
        @signal[appBundleReceivedHops];
        @signal[appBundleReceivedDelay];
        
        @statistic[appBundleSent](title="appBundleSent"; source="appBundleSent"; record=count; interpolationmode=none);
        @statistic[appBundleReceived](title="appBundleReceived"; source="appBundleReceived"; record=count; interpolationmode=none);
        @statistic[appBundleReceivedHops](title="appBundleReceivedHops"; source="appBundleReceivedHops"; record=mean,max,min,histogram; interpolationmode=none);
        @statistic[appBundleReceivedDelay](title="appBundleReceivedDelay"; source="appBundleReceivedDelay"; record=mean,max,min,histogram; interpolationmode=none);
                   
                      
    gates:
        inout gateToDtn;
}
