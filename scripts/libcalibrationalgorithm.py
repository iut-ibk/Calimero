import pycalimero
from calimeropublic import frange

class BruteForceSearch_P(pycalimero.ICalibrationAlg):
    def __init__(self):
        pycalimero.ICalibrationAlg.__init__(self)
        self.setDataType("parallel", pycalimero.UINT, "1")
        self.setDataType("clean results", pycalimero.BOOL, "1")
        
    def bruteforcesearch(self, calibrationvars, currentvar, env):
        var = calibrationvars[currentvar]
        
        for currentvalue in frange(var.min,var.max,var.step) + [var.max]:
            result = pycalimero.doublevector()
            result.append(currentvalue)
            var.setValues(result)
        
            if (currentvar==(calibrationvars.__len__()-1)):
                if (pycalimero.execIteration(calibrationvars) == False):
                    pycalimero.log("BruteForceSearch_P stoped by user",pycalimero.Standard)
                    return False
            else:
                if(self.bruteforcesearch(calibrationvars,currentvar+1,env)==False):
                    return True
            
        return True
              
    def start(self,calibrationvars, objectivevars, env, calibration):
        pycalimero.log("Start BruteForceSearch_P",pycalimero.Standard)
        result =  self.bruteforcesearch(calibrationvars,0,env)
        pycalimero.barrier()
        pycalimero.log("BruteForceSearch_P DONE",pycalimero.Standard)
        
        return result;
