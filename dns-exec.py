import subprocess
import sys

## ////////////////////////////////////////////////////////////

def help():
    print("missing command line arguments.")
    print("")
    print("usage:")
    print("    -f FILE        Input file to use for json parsing")
    return

## ////////////////////////////////////////////////////////////

def parseCommandLineArguments():
    argsLen = len(sys.argv)

    global filename
    global forceAws

    forceAws = False

    if argsLen < 3:
        help()
        return False

    for i in range(len(sys.argv)):
        # Always skip the first, it's the exe or python file
        if i == 0:
            continue

        if sys.argv[i] == "-f":
            filename = sys.argv[i + 1]
            i = i + 1
        elif sys.argv[i] == "-a":
            forceAws = True

    if not filename:
        help()
        return False

    return True

def main():
    import json
    
    if parseCommandLineArguments() == False:
        return False

    jsonFile = open(filename, "r")
    jsonStr = jsonFile.read()
    jsonListObj = json.loads(jsonStr)
    jsonFile.close()

    argsFormatString = "-d {domain} -s {server}"

    allInternalCorrect = True
    allExternalCorrect = True

    print()
    print("//////////////////////////////////////////////////")
    print("Attempting validations on internal network")
    print("//////////////////////////////////////////////////")
    print()

    ## Attempt validations from within the internal network
    for jsonObj in jsonListObj:
        print(jsonObj)
        if(jsonObj["internal"] == True):
            allInternalCorrect &= run("./dns-validation", argsFormatString.format(domain = jsonObj["domain"], server = jsonObj["server"]))
        else:
            allExternalCorrect &= run("./dns-validation", argsFormatString.format(domain = jsonObj["domain"], server = jsonObj["server"]))

    if forceAws == True & allExternalCorrect == True:
        return ValidateWithAws(jsonListObj)
        
    elif allInternalCorrect == True & allExternalCorrect == True:
        return True
    
    elif allExternalCorrect == True:
        print("Internal services validations had issues, but external services were ok.")
        return True

    # If we weren't able to validate allExternal, then we'll use aws lambda
    elif allInternalCorrect == False & allExternalCorrect == False:
        return ValidateWithAws(jsonListObj)

    return False

## ////////////////////////////////////////////////////////////
def ValidateWithAws(jsonListObj):    
    import json
    print()
    print("//////////////////////////////////////////////////")
    print("Attempting validations using AWS")
    print("//////////////////////////////////////////////////")
    print()

    allExternalCorrect = True

    outputList = []
    outputObj = dict
    for jsonObj in jsonListObj:
        if(jsonObj["internal"] == False):
            outputObj = {
                "server" : jsonObj["server"],
                "domain" : jsonObj["domain"]
            } 
            outputList.append(outputObj)

    tempFile = open("temp.json", "w")
    tempFile.write(json.dumps(outputList))
    tempFile.close()

    awsFormatString = "lambda invoke --function-name dns-validation --payload file://temp.json output.txt"
    returnVal = run("aws", awsFormatString)

    if returnVal == True:
        return readOutput()

    return False

## ////////////////////////////////////////////////////////////            

def readOutput():
    import json

    outputFile = open("output.txt", "r")
    jsonStr = outputFile.read()
    jsonListObj = json.loads(jsonStr)
    outputFile.close()

    allGood = True
    for jsonObj in jsonListObj:
        allGood &= jsonObj["result"]

    if(allGood):
        print("All validations from aws passed.")
    else:
        print("Somet validations failed, please see log for more details")
    return allGood

## ////////////////////////////////////////////////////////////            

def run(exe, args):
    subprocessStr = "{exe} {args}"
    subprocessStr = subprocessStr.format(exe = exe, args = args)
    try:
        print("running " + subprocessStr)
        completedProcessVal = subprocess.run(subprocessStr, shell=True, check=True)
        return True
    except subprocess.CalledProcessError:
        print("Process '" + subprocessStr + "' return a non-zero exit code.")
        return False

## ////////////////////////////////////////////////////////////            

retVal = main()
if retVal == True:
    exit(0)
else:
    exit(1)

## ////////////////////////////////////////////////////////////            