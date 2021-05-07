import matplotlib.pyplot as plt
import ast

INPUT_PATH = "/home/juanfraire/git/dtnsim/dtnsim/src/resultPlots/totin_jsac_example_plots/"

#METRIC = "Metric=appBundleReceived:count,General-"
METRIC = "Metric=routeCgrRouteTableEntriesCreated:max,General-"

#CONFIG = "volumeAware=allContacts,extensionBlock=on,contactPlan=global,returnToSender=false"
CONFIG = "volumeAware=allContacts,extensionBlock=on,contactPlan=local,returnToSender=false"
#CONFIG = "volumeAware=allContacts,extensionBlock=off,contactPlan=local,returnToSender=false"
#CONFIG = "volumeAware=1stContact,extensionBlock=off,contactPlan=local,returnToSender=false"

FILE_PATH_C1 = "routingType=oneBestPath," + CONFIG + ",bundlesNumber=50.txt"
FILE_PATH_C2 = "routingType=perNeighborBestPath," + CONFIG + ",bundlesNumber=50.txt"
FILE_PATH_C3 = "routingType=allPaths-firstEnding," + CONFIG + ",bundlesNumber=50.txt"
FILE_PATH_C4 = "routingType=allPaths-firstDepleted," + CONFIG + ",bundlesNumber=50.txt"
FILE_PATH_C5 = "routingType=allPaths-initial+anchor," + CONFIG + ",bundlesNumber=50.txt"

NAME_C1 = "oneBestPath"
NAME_C2 = "perNeighborBestPath"
NAME_C3 = "allPaths-firstEnding"
NAME_C4 = "allPaths-firstDepleted"
NAME_C5 = "allPaths-initial+anchor"

X_LABEL = "Bundles generated"
Y_LABEL = "Received bundles"

#FILE_OUTPUT_PATH = "/home/juanfraire/git/dtnsim/dtnsim/src/resultPlots/totin_jsac_appBundleReceived,routingType=all," + CONFIG + ".png"
FILE_OUTPUT_PATH = "/home/juanfraire/git/dtnsim/dtnsim/src/resultPlots/totin_jsac_routeCgrRouteTableEntriesCreated,routingType=all," + CONFIG + ".png"


# Markers: https://matplotlib.org/api/markers_api.html#module-matplotlib.markers
# Lines:   https://matplotlib.org/api/lines_api.html#matplotlib.lines.Line2D.set_linestyle

def main():
    c1 = getListFromFile(INPUT_PATH + METRIC + FILE_PATH_C1)
    c2 = getListFromFile(INPUT_PATH + METRIC + FILE_PATH_C2)
    c3 = getListFromFile(INPUT_PATH + METRIC + FILE_PATH_C3)
    c4 = getListFromFile(INPUT_PATH + METRIC + FILE_PATH_C4)
    c5 = getListFromFile(INPUT_PATH + METRIC + FILE_PATH_C5)

    # plot results
    plt.title(CONFIG, fontsize=9)
    # line_1, = plt.plot([x[0] for x in c1],[y[1] for y in c1],'--o', fillstyle='none', label=NAME_C1)
    # line_2, = plt.plot([x[0] for x in c2],[y[1] for y in c2],'-x', fillstyle='none', label=NAME_C2)
    line_3, = plt.plot([x[0] for x in c3],[y[1] for y in c3],'--v', fillstyle='none', label=NAME_C3)
    line_4, = plt.plot([x[0] for x in c4],[y[1] for y in c4],'-s', fillstyle='none', label=NAME_C4)
    line_5, = plt.plot([x[0] for x in c5],[y[1] for y in c5],'--+', fillstyle='none', label=NAME_C5)
    # plt.legend(handles=[line_1, line_2, line_3, line_4, line_5],loc=2)
    plt.legend(handles=[line_3, line_4, line_5],loc=2)
    plt.xlabel(X_LABEL)
    plt.ylabel(Y_LABEL)
    plt.ylim(0,40)
    plt.grid(color='gray', linestyle='dashed')
    plt.savefig(FILE_OUTPUT_PATH)
    plt.clf()
    plt.cla()
    plt.close()

''''
Get a list from file reading first line only
'''
def getListFromFile(path):
    with open(path) as f:
        lines = f.readlines()

    return ast.literal_eval(lines[0])

main()
