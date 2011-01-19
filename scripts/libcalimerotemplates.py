import pycalimero

class TemplateAlgorithm(pycalimero.ICalibrationAlg):
    def __init__(self):
        pycalimero.ICalibrationAlg.__init__(self)
        self.setDataType("max iterations", pycalimero.UINT, "30")
        self.setDataType("parallel", pycalimero.UINT, "1")
        self.setDataType("clean results", pycalimero.BOOL, "1")
        pycalimero.log("gaga", pycalimero.Error)
        
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
