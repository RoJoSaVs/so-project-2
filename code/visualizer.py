import json
import matplotlib.pyplot as plt


def stringToJsonData():
	with open("files/stats.txt", "r") as file:
		data = file.read().replace("'", '"')
		json_data = json.loads(data)
		return json_data


def createDictionary():
	jsonData = stringToJsonData()
	totalRequest = {}
	timeExecution = {}
	averageRequestTime = {}
	memoryConsumption = {}
	fourStat = {}
	for item in jsonData:
		totalRequest[item["server"]] = item["totalRequest"]
		timeExecution[item["server"]] = item["timeExecution"]
		averageRequestTime[item["server"]] = item["averageRequestTime"]
		memoryConsumption[item["server"]] = item["memoryConsumption"]
		fourStat[item["server"]] = item["fourStat"]

	return [totalRequest, timeExecution, averageRequestTime, memoryConsumption, fourStat]


def getColor(index):
	if(index == 0):
		return "green"
	elif(index == 1):
		return "blue"
	elif(index == 2):
		return "cyan"
	else:
		return "yellow"

def drawStats():
	ylabelValue = ["Time Execution", "Average Request Time", "Memory Consumption", "Four Stat"]
	dictArray = createDictionary()
	for index in range(0, len(dictArray) - 1):
		keys = list(dictArray[index].keys())
		values = list(dictArray[index].values())
		fig = plt.figure(index + 1, figsize = (5, 3))
		colorString = getColor(index)
		plt.bar(keys, values, color = colorString , width = 0.4)
		plt.xlabel("Total Request")
		plt.ylabel(ylabelValue[index])
		plt.title(ylabelValue[index] + " - Total Request")
	plt.show()


if __name__ == "__main__":
	drawStats()