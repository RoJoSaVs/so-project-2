import json

import matplotlib
import matplotlib.pyplot as plt
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
                "totalBytesSend": []}
    heavyDict = {"totalRequest": [], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [],
                 "totalBytesSend": []}
    threadDict = {"totalRequest": [], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [],
                  "totalBytesSend": []}
    preheavyDict = {"totalRequest": [], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [],
                    "totalBytesSend": []}
    for item in jsonData:
        if (item["server"] == "fifo"):
            fifoDict["totalRequest"].append(item["totalRequest"])
            fifoDict["timeExecution"].append(item["timeExecution"])
            fifoDict["averageRequestTime"].append(item["averageRequestTime"])
            fifoDict["memoryConsumption"].append(item["memoryConsumption"])
            fifoDict["totalBytesSend"].append(
                item["totalBytesSend"] / (item["totalRequest"] * item["timeExecution"] * 1000))

        elif (item["server"] == "heavy"):
            heavyDict["totalRequest"].append(item["totalRequest"])
            heavyDict["timeExecution"].append(item["timeExecution"])
            heavyDict["averageRequestTime"].append(item["averageRequestTime"])
            heavyDict["memoryConsumption"].append(item["memoryConsumption"])
            heavyDict["totalBytesSend"].append(
                item["totalBytesSend"] / (item["totalRequest"] * item["timeExecution"] * 1000))

        elif (item["server"] == "thread"):
            threadDict["totalRequest"].append(item["totalRequest"])
            threadDict["timeExecution"].append(item["timeExecution"])
            threadDict["averageRequestTime"].append(item["averageRequestTime"])
            threadDict["memoryConsumption"].append(item["memoryConsumption"])
            threadDict["totalBytesSend"].append(
                item["totalBytesSend"] / (item["totalRequest"] * item["timeExecution"] * 1000))

        elif (item["server"] == "preheavy"):
            preheavyDict["totalRequest"].append(item["totalRequest"])
            preheavyDict["timeExecution"].append(item["timeExecution"])
            preheavyDict["averageRequestTime"].append(item["averageRequestTime"])
            preheavyDict["memoryConsumption"].append(item["memoryConsumption"])
            preheavyDict["totalBytesSend"].append(
                item["totalBytesSend"] / (item["totalRequest"] * item["timeExecution"] * 1000))

    resultDict = {"fifoDict": fifoDict, "heavyDict": heavyDict,
                  "threadDict": threadDict, "preheavyDict": preheavyDict}

    return resultDict


def setGraphData(dict, axs):
    axs[dict["posX"], dict["posY"]].plot(dict["x"], dict["y"])
    axs[dict["posX"], dict["posY"]].plot(dict["data"]["fifoDict"][dict["xValues"]], dict["data"]["fifoDict"][dict["yValues"]],
                                         'og')  # fifo
    axs[dict["posX"], dict["posY"]].plot(dict["data"]["heavyDict"][dict["xValues"]], dict["data"]["heavyDict"][dict["yValues"]],
                                         'ob')  # heavy
    axs[dict["posX"], dict["posY"]].plot(dict["data"]["threadDict"][dict["xValues"]], dict["data"]["threadDict"][dict["yValues"]],
                                         'oc')  # thread
    axs[dict["posX"], dict["posY"]].plot(dict["data"]["preheavyDict"][dict["xValues"]], dict["data"]["preheavyDict"][dict["yValues"]],
                                         'oy')  # preheavy
    axs[dict["posX"], dict["posY"]].set_title(dict["title"])
    axs[dict["posX"], dict["posY"]].legend(['FIFO', 'Heavy', 'Thread', 'Pre-Heavy'],
                                           loc='upper right')  # set the position of the legend
    axs[dict["posX"], dict["posY"]].set(xlabel=dict["xLabel"], ylabel=dict["yLabel"])


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
                  "xLabel": "Time Execution", "yLabel": "Total Request",
                  "xValues": "timeExecution",
                  "yValues": "totalRequest",
                  "data": timeRequest}
    graph2Dict = {"posX": 0, "posY": 1, "x": x + 1, "y": y + 1,
                  "title": "Memory Consumption vs Request",
                  "xLabel": "Memory Consumption", "yLabel": "Total Request",
                  "xValues": "memoryConsumption",
                  "yValues": "totalRequest",
                  "data": stats}
    graph3Dict = {"posX": 1, "posY": 0, "x": x, "y": y ** 2,
                  "title": "Average Execution Time vs Request",
                  "xLabel": "Average Time Execution", "yLabel": "Total Request",
                  "xValues": "averageRequestTime",
                  "yValues": "totalRequest",
                  "data": stats}
    graph4Dict = {"posX": 1, "posY": 1, "x": x + 2, "y": y + 2,
                  "title": "Data Transfer Speed",
                  "xLabel": "timeExecution", "yLabel": "totalBytesSend",
                  "xValues": "timeExecution",
                  "yValues": "totalBytesSend",
                  "data": stats}

    setGraphData(graph1Dict, axs)
    setGraphData(graph2Dict, axs)
    setGraphData(graph3Dict, axs)
    setGraphData(graph4Dict, axs)

    fig.tight_layout()
    plt.show()


# Reset values after server execution
# writeToJson("../files/timeRequest.json")
# writeToJson("../files/stats.json")


if __name__ == "__main__":
    drawStats()
