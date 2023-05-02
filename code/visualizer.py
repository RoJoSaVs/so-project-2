import json

import matplotlib
import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
from pylab import *


def writeToJson(filename):
    f = open(filename, "w", encoding='ISO-8859-1')
    f.write("[]")


def stringToJsonData(filename):
    f = open(filename, "r", encoding='ISO-8859-1')
    data = json.load(f)
    return data


def getStats(filename):
    jsonData = stringToJsonData(filename)
    fifoDict = {"totalRequest": [], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [],
                "totalBytesSend": [0], "times": [0]}
    heavyDict = {"totalRequest": [], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [],
                 "totalBytesSend": [0], "times": [0]}
    threadDict = {"totalRequest": [], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [],
                  "totalBytesSend": [0], "times": [0]}
    preheavyDict = {"totalRequest": [], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [],
                    "totalBytesSend": [0], "times": [0]}
    for item in jsonData:
        if (item["server"] == "fifo"):
            fifoDict["totalRequest"].append(item["totalRequest"])
            fifoDict["timeExecution"].append(item["timeExecution"])
            fifoDict["averageRequestTime"].append(item["averageRequestTime"])
            fifoDict["memoryConsumption"].append(item["memoryConsumption"])
            fifoDict["totalBytesSend"].append(item["totalBytesSend"])
            fifoDict["times"].append((item["timeExecution"]) + fifoDict["times"][-1])

        elif (item["server"] == "heavy"):
            heavyDict["totalRequest"].append(item["totalRequest"])
            heavyDict["timeExecution"].append(item["timeExecution"])
            heavyDict["averageRequestTime"].append(item["averageRequestTime"])
            heavyDict["memoryConsumption"].append(item["memoryConsumption"])
            heavyDict["totalBytesSend"].append(item["totalBytesSend"])
            heavyDict["times"].append((item["timeExecution"]) + heavyDict["times"][-1])

        elif (item["server"] == "thread"):
            threadDict["totalRequest"].append(item["totalRequest"])
            threadDict["timeExecution"].append(item["timeExecution"])
            threadDict["averageRequestTime"].append(item["averageRequestTime"])
            threadDict["memoryConsumption"].append(item["memoryConsumption"])
            threadDict["totalBytesSend"].append(item["totalBytesSend"])
            threadDict["times"].append((item["timeExecution"]) + threadDict["times"][-1])

        elif (item["server"] == "preheavy"):
            preheavyDict["totalRequest"].append(item["totalRequest"])
            preheavyDict["timeExecution"].append(item["timeExecution"])
            preheavyDict["averageRequestTime"].append(item["averageRequestTime"])
            preheavyDict["memoryConsumption"].append(item["memoryConsumption"])
            preheavyDict["totalBytesSend"].append(item["totalBytesSend"])
            preheavyDict["times"].append((item["timeExecution"]) + preheavyDict["times"][-1])

    resultDict = {"fifoDict": fifoDict, "heavyDict": heavyDict,
                  "threadDict": threadDict, "preheavyDict": preheavyDict}

    return resultDict


def setGraphData(dictionary, axs, xValues, yValues, format = False):

    legends = ['FIFO', 'Heavy', 'Thread', 'Pre-Heavy']
    fifoXValues = dictionary["data"]["fifoDict"][xValues]
    fifoYValues = dictionary["data"]["fifoDict"][yValues]

    heavyXValues = dictionary["data"]["heavyDict"][xValues]
    heavyYValues = dictionary["data"]["heavyDict"][yValues]

    threadXValues = dictionary["data"]["threadDict"][xValues]
    threadYValues = dictionary["data"]["threadDict"][yValues]

    preheavyXValues = dictionary["data"]["preheavyDict"][xValues]
    preheavyYValues = dictionary["data"]["preheavyDict"][yValues]

    axs[dictionary["posX"], dictionary["posY"]].plot(fifoXValues, fifoYValues, 'c')
    axs[dictionary["posX"], dictionary["posY"]].plot(heavyXValues, heavyYValues, 'b')
    axs[dictionary["posX"], dictionary["posY"]].plot(threadXValues, threadYValues, 'g')
    axs[dictionary["posX"], dictionary["posY"]].plot(preheavyXValues, preheavyYValues, 'r')

    axs[dictionary["posX"], dictionary["posY"]].set_title(dictionary["title"])
    axs[dictionary["posX"], dictionary["posY"]].legend(legends, loc='upper right')  # set the position of the legend
    axs[dictionary["posX"], dictionary["posY"]].set(xlabel=dictionary["xLabel"], ylabel=dictionary["yLabel"])

    # formatear el eje y para mostrar los números en millones
    if format:
        axs[dictionary["posX"], dictionary["posY"]].get_yaxis().set_major_formatter(matplotlib.ticker.FuncFormatter(lambda x, p: (str(x/1000) + "k")))


def drawStats():
    # ylabelValue = ["Time Execution", "Average Request Time", "Memory Consumption", "Bandwidth"]
    timeRequest = getStats("./files/timeRequest.json")  # Incremental stats
    stats = getStats("./files/stats.json")  # Average Stats file


    x = 100
    y = 100
    # First Stat ExecutionTime vs Request
    fig, axs = plt.subplots(2, 2)

    graph1Dict = {"posX": 0, "posY": 0, "x": x, "y": y,
                  "title": "Execution Time vs Request",
                  "xLabel": "Total Request", "yLabel": "Time Execution",
                  "xValues": "totalRequest",
                  "yValues": "timeExecution",
                  "data": timeRequest}
    graph2Dict = {"posX": 0, "posY": 1, "x": x + 1, "y": y + 1,
                  "title": "Memory Consumption vs Request",
                  "xLabel": "Memory Consumption", "yLabel": "Total Request",
                  "xValues": "memoryConsumption",
                  "yValues": "totalRequest",
                  "data": timeRequest}
    graph3Dict = {"posX": 1, "posY": 0, "x": x, "y": y ** 2,
                  "title": "Average Execution Time vs Request",
                  "xLabel": "Average Time Execution", "yLabel": "Total Request",
                  "xValues": "averageRequestTime",
                  "yValues": "totalRequest",
                  "data": timeRequest}
    graph4Dict = {"posX": 1, "posY": 1, "x": x + 2, "y": y + 2,
                  "title": "Data Transfer Speed",
                  "xLabel": "timeExecution", "yLabel": "totalBytesSend",
                  "xValues": "times",
                  "yValues": "totalBytesSend",
                  "data": timeRequest}

    setGraphData(graph1Dict, axs, graph1Dict["xValues"], graph1Dict["yValues"])
    setGraphData(graph2Dict, axs, graph2Dict["xValues"], graph2Dict["yValues"])
    setGraphData(graph3Dict, axs, graph3Dict["xValues"], graph3Dict["yValues"])
    setGraphData(graph4Dict, axs, graph4Dict["xValues"], graph4Dict["yValues"], True)

    fig.tight_layout()
    plt.show()


# Reset values after server execution
# writeToJson("../files/timeRequest.json")
# writeToJson("../files/stats.json")


if __name__ == "__main__":
    drawStats()
