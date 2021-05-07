import sys
import matplotlib.pyplot as plt
from matplotlib import gridspec
import math

def main():

    inputFileLocation = "."
    outputFileLocation = "."
    if(len(sys.argv) > 1):
        inputFileLocation = sys.argv[1]
    if (len(sys.argv) > 2):
        outputFileLocation = sys.argv[2]

    import sqlite3
    import matplotlib.pyplot as plt

    print("Start Python Scalar Plot ...")

    runId = 1

    ### Plot scalar stats ###

    # Connect to database
    conn = sqlite3.connect(str(inputFileLocation) + str('/') + str('General-#0.sca'))
    conn.row_factory = sqlite3.Row
    cur = conn.cursor()

    # execute sql query to get nodesNumber
    cur.execute("SELECT * FROM runparam WHERE parName='dtnsim.nodesNumber'")
    rows0 = cur.fetchall()
    nodesNumber = rows0[0]["parValue"]

    # save stats in dictionaries
    # statsApp[statName][nodeNum] = [min, max, mean, std deviation]
    # statsDtn[statName][nodeNum] = [min, max, mean, std deviation]
    statsApp = {}
    statsDtn = {}

    cur.execute("SELECT * FROM statistic")
    rows = cur.fetchall()

    moduleNames = [row["moduleName"] for row in rows]
    statNames = [row["statName"] for row in rows]
    statIds = [row["statId"] for row in rows]
    statMins = [row["statMin"] for row in rows]
    statMaxs = [row["statMax"] for row in rows]
    statMeans = [row["statMean"] for row in rows]
    statStds = [row["statStddev"] for row in rows]

    for i in range(0, len(statIds)):

        moduleName = moduleNames[i]
        nodeNum = int(''.join(ele for ele in moduleName if ele.isdigit()))

        if (nodeNum != 0):
            values = [statMins[i], statMaxs[i], statMeans[i], statStds[i]]

            if(moduleName.find(".app") != -1):
                if(statNames[i] in statsApp.keys()):
                    d = statsApp[statNames[i]]
                    d[nodeNum] = values
                    statsApp[statNames[i]] = d
                else:
                    d = {}
                    d[nodeNum] = values
                    statsApp[statNames[i]] = d

            if(moduleName.find(".dtn") != -1):
                if(statNames[i] in statsDtn.keys()):
                    d = statsDtn[statNames[i]]
                    d[nodeNum] = values
                    statsDtn[statNames[i]] = d
                else:
                    d = {}
                    d[nodeNum] = values
                    statsDtn[statNames[i]] = d

    cols = 2
    rows = int(math.ceil(len(statsApp) / cols))
    gs = gridspec.GridSpec(rows, cols)
    fig = plt.figure()

    k = 0
    for key in statsApp:
        d = statsApp[key]
        ax = fig.add_subplot(gs[k])
        k = k+1

        plt.xlabel('node eid')
        plt.ylabel(key)
        plt.grid()
        ax.set_xticks(range(1, int(nodesNumber)+1))

        for key2 in d:
            values = d[key2]
            min = values[0]
            max = values[1]
            mean = values[2]
            std = values[3]

            plt.errorbar(key2, mean, xerr=0.0, yerr=std, fmt='o')

    fig.tight_layout()
    fileStr = str(outputFileLocation) + str("/") + str("dtnsim.ScalarStats.App") + str(".pdf")
    plt.savefig(fileStr, format='pdf')

    cols = 2
    rows = int(math.ceil(len(statsDtn) / cols))
    gs = gridspec.GridSpec(rows, cols)
    fig = plt.figure()

    k = 0
    for key in statsDtn:
        d = statsDtn[key]
        ax = fig.add_subplot(gs[k])
        k = k+1

        plt.xlabel('node eid')
        plt.ylabel(key)
        plt.grid()
        ax.set_xticks(range(1, int(nodesNumber)+1))

        for key2 in d:
            values = d[key2]
            min = values[0]
            max = values[1]
            mean = values[2]
            std = values[3]

            plt.errorbar(key2, mean, xerr=0.0, yerr=std, fmt='o')

    fig.tight_layout()
    fileStr = str(outputFileLocation) + str("/") + str("dtnsim.ScalarStats.Dtn") + str(".pdf")
    plt.savefig(fileStr, format='pdf')

    print("End Python Scalar Plot.")

if __name__ == "__main__":
    main()
