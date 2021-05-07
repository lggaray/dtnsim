import pandas as pd
import pickle
import sys

nNodes = 7

def create_vec_dict(path, sim):
    df = pd.read_csv(path)
    dic = dict()

    #seleccionamos solos los vectores
    vecs = df.loc[df['type'] == 'vector']

    grouped = vecs.groupby(vecs.module)

    for i in range(nNodes):
        dic[i+1] = grouped.get_group("dtnsim.node[{}].dtn".format(i+1))

    with open('dicts/vec_dict{}.pickle'.format(sim), 'wb') as handle:
        pickle.dump(dic, handle, protocol=pickle.HIGHEST_PROTOCOL)

#get the variables
path, sim = sys.argv[1], int(sys.argv[2])

create_vec_dict(path, sim)
