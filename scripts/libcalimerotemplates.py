import pycalimero
import time

class TemplateAlgorithm(pycalimero.ICalibrationAlg):
    def __init__(self):
        pycalimero.ICalibrationAlg.__init__(self)
        self.setDataType("max iterations", pycalimero.UINT, "30")
        self.setDataType("parallel", pycalimero.UINT, "1")
        self.setDataType("clean results", pycalimero.BOOL, "1")
        
    def start(self,calibrationvars, objectivevars, env, calibration):
        pycalimero.log("Start TemplateAlgorithm",pycalimero.Standard)
        
        for i in range(0,int(self.getValueOfParameter("max iterations"))):
            #pycalimero.log("Run iteration",pycalimero.LogLevel.standard)
            if (pycalimero.execIteration(calibrationvars) == False):
                pycalimero.log("TemplateAlgorithm stoped by user",pycalimero.Standard)
                return True

            pycalimero.barrier()
            #pycalimero.log("Finish iteration",pycalimero.LogLevel.standard)
            
        pycalimero.log("TemplateAlgorithm DONE",pycalimero.Standard)
        return True
    
class TemplateModelSimulator(pycalimero.IModelSimulator):
    def __init__(self):
        pycalimero.IModelSimulator.__init__(self)
        
    def execModel(self,domain):
        return True
        
class DummyModel_P(pycalimero.IModelSimulator):
    def __init__(self):
        pycalimero.IModelSimulator.__init__(self)
        self.setDataType("fac n", pycalimero.UINT, "0")
        self.setDataType("iterations", pycalimero.UINT, "10000")
    def fac(self,x):
        if (x < 2):
            return x
        else:
            return (x * self.fac(x-1))

    def execModel(self,domain):
        facn = int(self.getValueOfParameter("fac n"))
        iterations = int(self.getValueOfParameter("iterations"))
        for i in range(iterations):
            tmp = self.fac(facn)
        return True
