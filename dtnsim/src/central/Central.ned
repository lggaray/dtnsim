package src.central;

simple Central
{
    parameters:
        
        @class(dtnsim::Central);
        
        bool saveTopology = default(false);
        bool saveFlows = default(false);
        bool saveLpFlows = default(false);
        string contactsFile = default("contacts.txt");
        
        bool enableAvailableRoutesCalculation = default(false);
        int deleteNContacts = default(0);
		bool useCentrality = default(false);
		bool faultsAware = default(true);
		
		@signal[contactsNumber];
		@signal[totalRoutes];
		@signal[availableRoutes];
		@signal[pairsOfNodesWithAtLeastOneRoute];
        @statistic[contactsNumber](title="contactsNumber"; source="contactsNumber"; record=sum; interpolationmode=none);
        @statistic[totalRoutes](title="totalRoutes"; source="totalRoutes"; record=sum; interpolationmode=none);
        @statistic[availableRoutes](title="availableRoutes"; source="availableRoutes"; record=sum; interpolationmode=none);
        @statistic[pairsOfNodesWithAtLeastOneRoute](title="pairsOfNodesWithAtLeastOneRoute"; source="pairsOfNodesWithAtLeastOneRoute"; record=sum; interpolationmode=none);
        
        @display("i=device/terminal");
}
