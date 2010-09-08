import pycalimero

def frange(start,end,step):
    return map(lambda x: x*step, range(int(start*1./step),int(end*1./step)))

def findBestIterationNumber(r,name):
    results = r
    bestresult = 0
    bestiterationnumber = -1
    
    if(not(results.__len__())):
        return -1
    
    for result in results:
        currentresultvector = result.getObjectiveFunctionParameterResults(name)
        if (bestiterationnumber==-1):
            bestresult = currentresultvector[0]
            bestiterationnumber = result.getIterationNumber()
        else:
            if(bestresult > abs(currentresultvector[0])):
                bestresult = currentresultvector[0]
                bestiterationnumber = result.getIterationNumber()
            
    return bestiterationnumber