import pandas as pd
import pickle
import sys

nNodes = 7


'''
'appBundleReceivedDelay:mean', 'appBundleReceivedDelay:max', 'appBundleReceivedDelay:min', 'appBundleReceivedDelay:histogram'
'''

features = ['routeCgrRouteTableEntriesCreated:max',
            'routeCgrRouteTableEntriesCreated:sum',
            'sdrBytesStored:timeavg',
            'sdrBytesStored:max',
            'sdrBytesStored:sum',
            'sdrBundleStored:timeavg', 
            'sdrBundleStored:max',
            'dtnBundleReRouted:count', 
            'dtnBundleReceivedFromCom:count',
            'dtnBundleSentToAppRevisitedHops:histogram',
            'dtnBundleSentToAppHopCount:histogram',
            'dtnCgrCalls:count',
            'sdrBundlesDeleted:max'
            ]


def create_sca_dict(path, sim):
    df = pd.read_csv(path)
    dic = dict()
    feat_dic = dict()

    grouped = df.groupby(df.module)

    for i in range(nNodes):
        app = grouped.get_group("dtnsim.node[{}].app".format(i+1))
        dtn = grouped.get_group("dtnsim.node[{}].dtn".format(i+1))
        #dic['node {}'.format(i+1)] = app.append(dtn) #lo guardo en forma 'node i'
        dic[i+1] = app.append(dtn)

    for k,v in dic.items():
        hist = v.loc[v['type'] == 'histogram']
        sca = v.loc[v['type'] == 'scalar']
        frames = []
        for f in features:
            frames.append(hist.loc[hist['name'] == f])
            frames.append(sca.loc[sca['name'] == f])    
        feat_dic[k] = pd.concat(frames)

    with open('dicts/sca_dict{}.pickle'.format(sim), 'wb') as handle:
        pickle.dump(feat_dic, handle, protocol=pickle.HIGHEST_PROTOCOL)


path, sim = sys.argv[1], int(sys.argv[2])

create_sca_dict(path, sim)
