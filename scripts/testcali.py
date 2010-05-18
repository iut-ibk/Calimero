import pycalimero

class MyOF(pycalimero.IObjectiveFunction):
	def __init__(self):
		pycalimero.IObjectiveFunction.__init__(self)

	def eval(self):
		pycalimero.log("eval called in %s" % self.__class__.__name__)
		print "fein eval called in %s" % self.__class__.__name__
		return pycalimero.doublevector()



class MyOF1(pycalimero.IObjectiveFunction):
	def __init__(self):
		pycalimero.IObjectiveFunction.__init__(self)
		self.setDataType("testparameter",pycalimero.DATATYPE.DOUBLE)
		pycalimero.log("init done in %s" % self.__class__.__name__)
		

	def eval(self,iterationparameters, observedparameters, objectivefunctionparameters):
		pycalimero.log("eval called in %s" % self.__class__.__name__)
		#pycalimero.IObjectiveFunction.getValueOfParameter(self,"testpar")
		pycalimero.log("eval called in %s" % self.__class__.__name__)
		return pycalimero.doublevector()
