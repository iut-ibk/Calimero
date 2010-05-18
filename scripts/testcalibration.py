import pycalimero

class MyCalibration(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("testparameter",pycalimero.DATATYPE.DOUBLE)

	def start(self):
		pycalimero.log("fein eval called in %s" % self.__class__.__name__)

