import pycalimero
import math

class Sum(pycalimero.IObjectiveFunction):
    def __init__(self):
        pycalimero.IObjectiveFunction.__init__(self)
        
    def eval(self,iterationparameters, observedparameters, objectivefunctionparameters):
        result = pycalimero.doublevector()
        resultvalue = 0.0
        allparameters = []
        allparameters.extend(iterationparameters)
        allparameters.extend(observedparameters)
        allparameters.extend(objectivefunctionparameters)
        
        for i in allparameters:
            resultvalue += sum(i.getValues())

        result.push_back(resultvalue)
        return result
     
class DummyOFunction_P(pycalimero.IObjectiveFunction):
    def __init__(self):
        pycalimero.IObjectiveFunction.__init__(self)
        self.setDataType("fac n", pycalimero.UINT, "0")
        self.setDataType("iterations", pycalimero.UINT, "1000")
        
    def eval(self,iterationparameters, observedparameters, objectivefunctionparameters):
        result = pycalimero.doublevector()
        resultvalue = 1.0
        result.push_back(resultvalue)
        
        iterations = int(self.getValueOfParameter("iterations"))
        n = int(self.getValueOfParameter("fac n"))
        
        for i in range(iterations):
            for ni in range(n):
                resultvalue *= (ni+1)

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
            pycalimero.log("Only one vector is allowed in ReverseVector", pycalimero.Warning)
        
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
        #search for vectors and check their size
        vectors = pycalimero.doublevectorvector()
        
        for var in iterationparameters:
            vectors.append(var.getValues())
        
        for var in observedparameters:
            vectors.append(var.getValues())
        
        
        for var in objectivefunctionparameters:
            vectors.append(var.getValues())
        
        if(vectors.__len__()!=2):
            pycalimero.log("Only two vectors are allowed in VectorErrorSquare", pycalimero.Error)
            return pycalimero.doublevector(1,999999999999999)
        
        
        if(vectors[0].__len__() < vectors[1].__len__()):
            tmp = pycalimero.doublevector(vectors[0])
            while(tmp.__len__() < vectors[1].__len__()): 
                tmp.append(999)
            
            vectors[0]=tmp;
   
        if(vectors[0].__len__() > vectors[1].__len__()):
            tmp = pycalimero.doublevector(vectors[1])
            while(tmp.__len__() < vectors[0].__len__()): 
                tmp.append(999)
            
            vectors[1]=tmp;
            
        #calculate
        result = pycalimero.doublevector(vectors[0].__len__())
        
        for index,value1 in enumerate(vectors[0]):
            result[index] = math.pow(value1 - vectors[1][index],2)
        
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
            pycalimero.log("Only two vectors are allowed in VectorError", pycalimero.Warning)
        
        #search for vectors and check their size
        vectors = pycalimero.doublevectorvector()
        
        for var in iterationparameters:
            vectors.append(var.getValues())
        
        for var in observedparameters:
            vectors.append(var.getValues())
        
        
        for var in objectivefunctionparameters:
            vectors.append(var.getValues())
        
        if(vectors[0].__len__()!=vectors[1].__len__()):
            pycalimero.log("Vectors do not have the same size in VectorError", pycalimero.Warning)

        #calculate
        result = pycalimero.doublevector()
        
        index = 0
        for value1 in vectors[0]:
            currentresult = value1 - vectors[1][index]
            result.append(currentresult)
            index = index+1
        
        return result   
    
