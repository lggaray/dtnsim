import sys

def main():

    inputFileLocation = "."
    outputFileLocation = "."
    if(len(sys.argv) > 1):
        inputFileLocation = sys.argv[1]
    if (len(sys.argv) > 2):
        outputFileLocation = sys.argv[2]

    import sqlite3
    import matplotlib.pyplot as plt

    print("Start Python Vectorial Plot ...")

    runId = 1

    ### Plot vectorial stats ###

    # Connect to database
    conn = sqlite3.connect(str(inputFileLocation) + str('/') + str('General-#0.vec'))
    conn.row_factory = sqlite3.Row
    cur = conn.cursor()

    # execute sql query to get nodesNumber
    cur.execute("SELECT * FROM runparam WHERE parName='dtnsim.nodesNumber'")
    rows0 = cur.fetchall()
    nodesNumber = rows0[0]["parValue"]

    # plot stats for each node
    for n in range(1, (int(nodesNumber) + 1)):

        nodesNumberStr = 'dtnsim.node[%s].dtn' % str(n)

        # get moduleNames, vectorNames and vectorIds of each vectorial stat
        cur.execute("SELECT * FROM vector WHERE moduleName='%s'" % nodesNumberStr)
        rows = cur.fetchall()

        moduleNames = [row["moduleName"] for row in rows]
        vectorNames = [row["vectorName"] for row in rows]
        vectorIds = [row["vectorId"] for row in rows]

        # print(moduleNames)
        # print(vectorNames)
        # print(vectorIds)

        # gets sim time exponent
        # necessary to obtain the real simulation time from the "simTimeRaw"
        # written to the .vec file
        cur.execute("SELECT * FROM run WHERE runId=?", str(runId))
        rows2 = cur.fetchall()
        simtimeExpQ = [row["simtimeExp"] for row in rows2]
        simtimeExp = simtimeExpQ[0]

        # plot each stat from the current node
        for i in range(0, len(vectorIds)):
            id = vectorIds[i]

            # gets data for this stat and saves it in dataX and dataY
            cur.execute("SELECT * FROM vectordata WHERE vectorId='%s'" % str(id))
            rows3 = cur.fetchall()

            dataX = [row["simTimeRaw"] * (10 ** simtimeExp) for row in rows3]
            dataY = [row["value"] for row in rows3]

            fig = plt.figure()
            plt.plot(dataX, dataY, '--bo')
            plt.title(str(moduleNames[i]))
            plt.xlabel('sim time [s]')
            plt.ylabel(str(vectorNames[i]))
            plt.grid()
            # plt.show()
            fileStr = str(outputFileLocation) + str("/") + str(moduleNames[i]) + str(';') + str(vectorNames[i] + str(".pdf"))
            plt.savefig(fileStr, format='pdf')

    print("End Python Vectorial Plot.")

if __name__ == "__main__":
    main()
