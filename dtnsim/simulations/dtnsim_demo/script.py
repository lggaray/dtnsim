from configparser import ConfigParser
from random import randrange, choice, randint
from random import choice
import numpy as np
import sys, math, collections, random

'''
input (T,X)
T: every how many seconds starts a transmission
X: total traffic generated in the simulation 
'''

#T = 14400
#X = 61440

def num_pieces(num,lenght):
    ot = list(range(1,lenght+1))[::-1]
    all_list = []
    for i in range(lenght-1):
        n = random.randint(1, num-ot[i])
        all_list.append(n)
        num -= n
    all_list.append(num)
    #random.shuffle(all_list)
    return all_list


def split_given_number_into_n_random_numbers(number, number_of_subsections, min_random_number_desired = 1):
    final_list = []
    cumulative_sum_of_random_numbers = 0
    current_subsection = 1
    max_random_number = int(number/number_of_subsections)
    if min_random_number_desired > max_random_number:
        print("ERROR: Cannot have min number as {} and split {} in {} subsections".format(min_random_number_desired,
                                                                                          number, number_of_subsections))
        return False
    while (True):
        random_number = random.randint(min_random_number_desired, max_random_number)
        final_list.append(random_number)
        cumulative_sum_of_random_numbers += random_number
        number -= random_number
        current_subsection += 1
        if current_subsection == number_of_subsections:
            random_number = number
            final_list.append(random_number)
            cumulative_sum_of_random_numbers += random_number
            break
    #print("Final cumulative sum of random numbers = {}".format(cumulative_sum_of_random_numbers))
    random.shuffle(final_list)
    return final_list


def create_traffic_patterns(T, X, sim):
    sending_nodes = 6
    dest_node = 7

    sim_time = 86400 # simulation time, one day
    bundle_size = 128 #bytes
    n_bundles = X / bundle_size
    intervals = math.ceil(sim_time / T) 
    #n_bundles = math.ceil(X / bundle_size / intervals) # #bundles per interval
    #start_times_list = list(np.arange(T, sim_time+1, T)) #list of times the node generate traffic

    config = ConfigParser(allow_no_value=True)
    config.optionxform = str
    config.read("dtnsim_demo.ini")

    #bun_x_intervals_list = num_pieces(n_bundles, int(intervals))
    bun_x_intervals_list = split_given_number_into_n_random_numbers(n_bundles, intervals)
    
    confi_dic = collections.defaultdict(dict)
    for n in range(sending_nodes):
        confi_dic[n+1]['bundles'] = []
        confi_dic[n+1]['start'] = []
        confi_dic[n+1]['dest'] = []
        confi_dic[n+1]['size'] = []


    for idx, n_bun in enumerate(bun_x_intervals_list):
        orig = randint(1, sending_nodes)
        confi_dic[orig]['bundles'].append(int(n_bun))
        confi_dic[orig]['start'].append(T * idx+1)
        confi_dic[orig]['dest'].append(dest_node)
        confi_dic[orig]['size'].append(bundle_size)

    for k,v in confi_dic.items():
        config.set('General', 'dtnsim.node[{}].app.enable'.format(k), 'true')
        config.set('General', 'dtnsim.node[{}].app.bundlesNumber'.format(k), '"{}"'.format(', '.join(map(str, v['bundles']))))
        config.set('General', 'dtnsim.node[{}].app.start'.format(k), '"{}"'.format(', '.join(map(str, v['start']))))
        config.set('General', 'dtnsim.node[{}].app.destinationEid'.format(k), '"{}"'.format(', '.join(map(str, v['dest']))))
        config.set('General', 'dtnsim.node[{}].app.size'.format(k), '"{}"'.format(', '.join(map(str, v['size']))))


    with open('traffic{}.ini'.format(sim), 'w') as configfile:
        config.write(configfile)

    with open('tx{}.txt'.format(sim), 'w') as tx:
        tx.write('T= {}\nX= {}'.format(T,X))

#get the parameters
T, X, sim = int(sys.argv[1]), int(sys.argv[2]), int(sys.argv[3])

create_traffic_patterns(T, X, sim)
