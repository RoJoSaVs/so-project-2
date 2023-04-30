import json
import matplotlib.pyplot as plt


def writeToJson(filename):
	f = open(filename, "w")
	f.write("[]")


def stringToJsonData(filename):
	f = open(filename, "r")
	data = json.load(f)
	return data


def getStats(filename):
	jsonData = stringToJsonData(filename)
	fifoDict = {"totalRequest":	[], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [], "totalBytesSend": []}
	heavyDict = {"totalRequest":	[], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [], "totalBytesSend": []}
	threadDict = {"totalRequest":	[], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [], "totalBytesSend": []}
	preheavyDict = {"totalRequest":	[], "timeExecution": [], "averageRequestTime": [], "memoryConsumption": [], "totalBytesSend": []}
	for item in jsonData:
		if(item["server"] == "fifo"):
			fifoDict["totalRequest"].append(item["totalRequest"])
			fifoDict["timeExecution"].append(item["timeExecution"])
			fifoDict["averageRequestTime"].append(item["averageRequestTime"])
			fifoDict["memoryConsumption"].append(item["memoryConsumption"])
			fifoDict["totalBytesSend"].append(item["totalBytesSend"] / (item["totalRequest"] * item["timeExecution"] * 1000))

		elif(item["server"] == "heavy"):
			heavyDict["totalRequest"].append(item["totalRequest"])
			heavyDict["timeExecution"].append(item["timeExecution"])
			heavyDict["averageRequestTime"].append(item["averageRequestTime"])
			heavyDict["memoryConsumption"].append(item["memoryConsumption"])
			heavyDict["totalBytesSend"].append(item["totalBytesSend"] / (item["totalRequest"] * item["timeExecution"] * 1000))

		elif(item["server"] == "thread"):
			threadDict["totalRequest"].append(item["totalRequest"])
			threadDict["timeExecution"].append(item["timeExecution"])
			threadDict["averageRequestTime"].append(item["averageRequestTime"])
			threadDict["memoryConsumption"].append(item["memoryConsumption"])
			threadDict["totalBytesSend"].append(item["totalBytesSend"] / (item["totalRequest"] * item["timeExecution"] * 1000))

		elif(item["server"] == "preheavy"):
			preheavyDict["totalRequest"].append(item["totalRequest"])
			preheavyDict["timeExecution"].append(item["timeExecution"])
			preheavyDict["averageRequestTime"].append(item["averageRequestTime"])
			preheavyDict["memoryConsumption"].append(item["memoryConsumption"])
			preheavyDict["totalBytesSend"].append(item["totalBytesSend"] / (item["totalRequest"] * item["timeExecution"] * 1000))

	return[fifoDict, heavyDict, threadDict, preheavyDict]


def drawStats():
	# ylabelValue = ["Time Execution", "Average Request Time", "Memory Consumption", "Bandwidth"]
	timeRequest = getStats("../files/timeRequest.json") # Incremental stats
	stats = getStats("../files/stats.json") # Average Stats file

	# First Stat ExecutionTime vs Request
	plt.figure(1, figsize = (5, 4))
	plt.plot(timeRequest[0]["totalRequest"], timeRequest[0]["timeExecution"], 'og') #fifo
	plt.plot(timeRequest[1]["totalRequest"], timeRequest[1]["timeExecution"], 'ob') #heavy
	plt.plot(timeRequest[2]["totalRequest"], timeRequest[2]["timeExecution"], 'oc') #thread
	plt.plot(timeRequest[3]["totalRequest"], timeRequest[3]["timeExecution"], 'oy') #preheavy

	# Second Stat AverageExecutionTime vs Request
	plt.figure(2, figsize = (5, 4))
	plt.plot(stats[0]["totalRequest"], stats[0]["timeExecution"], 'og') #fifo
	plt.plot(stats[1]["totalRequest"], stats[1]["timeExecution"], 'ob') #heavy
	plt.plot(stats[2]["totalRequest"], stats[2]["timeExecution"], 'oc') #thread
	plt.plot(stats[3]["totalRequest"], stats[3]["timeExecution"], 'oy') #preheavy
	
	# Third Stat MemoryConsuption vs Request
	plt.figure(3, figsize = (5, 4))
	plt.plot(stats[0]["totalRequest"], stats[0]["memoryConsumption"], 'og') #fifo
	plt.plot(stats[1]["totalRequest"], stats[1]["memoryConsumption"], 'ob') #heavy
	plt.plot(stats[2]["totalRequest"], stats[2]["memoryConsumption"], 'oc') #thread
	plt.plot(stats[3]["totalRequest"], stats[3]["memoryConsumption"], 'oy') #preheavy

	# Fourth stat
	plt.figure(4, figsize = (5, 4))
	plt.plot(timeRequest[0]["totalRequest"], timeRequest[0]["totalBytesSend"], 'og') #fifo
	plt.plot(timeRequest[1]["totalRequest"], timeRequest[1]["totalBytesSend"], 'ob') #heavy
	plt.plot(timeRequest[2]["totalRequest"], timeRequest[2]["totalBytesSend"], 'oc') #thread
	plt.plot(timeRequest[3]["totalRequest"], timeRequest[3]["totalBytesSend"], 'oy') #preheavy

	plt.show()

	# Reset values after server execution
	# writeToJson("../files/timeRequest.json")
	# writeToJson("../files/stats.json")


if __name__ == "__main__":
	drawStats()