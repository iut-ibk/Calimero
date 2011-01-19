import pycalimero
import math

class Sum(pycalimero.IObjectiveFunction):
    def __init__(self):
        pycalimero.IObjectiveFunction.__init__(self)
        
    def eval(self,iterationparameters, observedparameters, objectivefunctionparameters):
        result = pycalimero.doublevector()
        resultvalue = 0.0

	print type(iterationparameters)
	print type(result)
	for i in range(0,iterationparameters.size()):
		currentvec=iterationparameters[i].getValues()
		for vi in range(0,currentvec.size()):
			resultvalue += currentvec[vi]

	for i in range(0,observedparameters.size()):
		currentvec=observedparameters[i].getValues()
		for vi in range(0,currentvec.size()):
			resultvalue += currentvec[vi]

	for i in range(0,objectivefunctionparameters.size()):
		currentvec=objectivefunctionparameters[i].getValues()
		for vi in range(0,currentvec.size()):
			resultvalue += currentvec[vi]

        result.append(resultvalue)
        return result
    
class ReverseVector(pycalimero.IObjectiveFunction):
    def __init__(self):
        pycalimero.IObjectiveFunction.__init__(self)
        
    def eval(self,iterationparameters, observedparameters, objectivefunctionparameters):
        #search for number of vectors
        numberofvectors = 0
        numberofvectors = numberofvectors+iterationparameters.__len__()
        numberofvectors = numberofvectors+observedparameters.__len__()
        numberofvectors = numberofvectors+objectivefunctionparameters.__len__()
        
        if(numberofvectors!=1):
            pycalimero.log("Only one vector is allowed in ReverseVector", pycalimero.LogLevel.warning)
        
        #search for vectors and check their size
        vectors = pycalimero.doublevectorvector()
        
        for var in iterationparameters:
            vectors.append(var.getValues())
        
        for var in observedparameters:
            vectors.append(var.getValues())
        
        
        for var in objectivefunctionparameters:
            vectors.append(var.getValues())

        #calculate
        result = pycalimero.doublevector()
        
        for value in vectors[0]:
            result.append(-value)
            
        return result
    
class VectorErrorSquare(pycalimero.IObjectiveFunction):
    def __init__(self):
        pycalimero.IObjectiveFunction.__init__(self)
        
    def eval(self,iterationparameters, observedparameters, objectivefunctionparameters):
        #search for number of vectors
        numberofvectors = 0
        numberofvectors = numberofvectors+iterationparameters.__len__()
        numberofvectors = numberofvectors+observedparameters.__len__()
        numberofvectors = numberofvectors+objectivefunctionparameters.__len__()
        
        if(numberofvectors!=2):
            pycalimero.log("Only two vectors are allowed in VectorErrorSquare", pycalimero.LogLevel.warning)
        
        #search for vectors and check their size
        vectors = pycalimero.doublevectorvector()
        
        for var in iterationparameters:
            vectors.append(var.getValues())
        
        for var in observedparameters:
            vectors.append(var.getValues())
        
        
        for var in objectivefunctionparameters:
            vectors.append(var.getValues())
        
        if(vectors[0].__len__()!=vectors[1].__len__()):
            pycalimero.log("Vectors do not have the same size in VectorErrorSquare. Fill missing elements with a high value", pycalimero.LogLevel.warning)

        #calculate
        result = pycalimero.doublevector()
        
        index = 0
        for value1 in vectors[0]:
            currentresult = math.pow(value1 - vectors[1][index],2)
            result.append(currentresult)
            index = index+1
        
        size = max(vectors[0].__len__(),vectors[1].__len__())
        while(result.__len__() < size):
            result.append(999999999999999)
        
        return result
 
class VectorError(pycalimero.IObjectiveFunction):
    def __init__(self):
        pycalimero.IObjectiveFunction.__init__(self)
        
    def eval(self,iterationparameters, observedparameters, objectivefunctionparameters):
        #search for number of vectors
        numberofvectors = 0
        numberofvectors = numberofvectors+iterationparameters.__len__()
        numberofvectors = numberofvectors+observedparameters.__len__()
        numberofvectors = numberofvectors+objectivefunctionparameters.__len__()
        
        if(numberofvectors!=2):
            pycalimero.log("Only two vectors are allowed in VectorError", pycalimero.LogLevel.warning)
        
        #search for vectors and check their size
        vectors = pycalimero.doublevectorvector()
        
        for var in iterationparameters:
            vectors.append(var.getValues())
        
        for var in observedparameters:
            vectors.append(var.getValues())
        
        
        for var in objectivefunctionparameters:
            vectors.append(var.getValues())
        
        if(vectors[0].__len__()!=vectors[1].__len__()):
            pycalimero.log("Vectors do not have the same size in VectorError", pycalimero.LogLevel.warning)

        #calculate
        result = pycalimero.doublevector()
        
        index = 0
        for value1 in vectors[0]:
            currentresult = value1 - vectors[1][index]
            result.append(currentresult)
            index = index+1
        
        return result   
    
