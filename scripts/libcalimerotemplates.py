import pycalimero

class TemplateAlgorithm(pycalimero.ICalibrationAlg):
    def __init__(self):
        pycalimero.ICalibrationAlg.__init__(self)
        self.setDataType("max iterations", pycalimero.DATATYPE.UINT, "30")
        self.setDataType("parallel", pycalimero.DATATYPE.UINT, "1")
        self.setDataType("clean results", pycalimero.DATATYPE.BOOL, "1")
        
        
    def start(self,calibrationvars, objectivevars, env, calibration):
        pycalimero.log("Start TemplateAlgorithm",pycalimero.LogLevel.standard)
        
        for i in range(0,int(self.getValueOfParameter("max iterations"))):
            pycalimero.log("Run iteration",pycalimero.LogLevel.standard)
            env.execIteration(calibrationvars)
            env.barrier()
            pycalimero.log("Finish iteration",pycalimero.LogLevel.standard)
            
        pycalimero.log("TemplateAlgorithm DONE",pycalimero.LogLevel.standard)
        return True
    
class TemplateModelSimulator(pycalimero.IModelSimulator):
    def __init__(self):
        pycalimero.IModelSimulator.__init__(self)
        
    def execModel(self,domain):
        return True
