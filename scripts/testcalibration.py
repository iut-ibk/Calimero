import pycalimero

class MyCalibration(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("testparameter",pycalimero.DATATYPE.DOUBLE)
		self.setDataType("parallel",pycalimero.DATATYPE.DOUBLE)
		self.setValueOfParameter("parallel","1")
		self.setDataType("stringparameter",pycalimero.DATATYPE.STRING)

	def start(self,newcalpars,newopars,calibrationenv,calibration):
		pycalimero.log("fein eval called in %s" % self.__class__.__name__)
		index = 0
		while(calibrationenv.execIteration(newcalpars)):
			index = index+1
			
		return True

