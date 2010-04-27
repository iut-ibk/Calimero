import pycalimero

class Sum(pycalimero.IObjectiveFunction):
	def __init__(self):
		pycalimero.IObjectiveFunction.__init__(self)
		pycalimero.log("init done in %s" % self.__class__.__name__)
		
	def eval(self,iterationparameters, observedparameters, objectivefunctionparameters):
		pycalimero.log("eval called in %s" % self.__class__.__name__)
		result = pycalimero.doublevector()
		resultvalue = 0.0
		for var in iterationparameters:
			for value in var.getValues():
				resultvalue += value

		for var in observedparameters:
			for value in var.getValues():
				resultvalue += value

		for var in objectivefunctionparameters:
			for value in var.getValues():
				resultvalue += value

		result.append(resultvalue)
		return result
