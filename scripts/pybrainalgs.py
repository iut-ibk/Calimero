from random import random
from pybrain.structure.evolvables.evolvable import Evolvable
import pycalimero
from calimeropublic import frange
import numpy
from numpy import *
from pybrain.optimization import *
import sys

error = False
baseevo = None

def objf(x):
	xpars = array([],float32)
	calibrationvars=None
	calibration=None
	objectivevars=None
	env = None
	global error
	global baseevo
	
	if isinstance(x,Evolvable):
		xpars=x.x
		calibrationvars=x.calibrationvars
		calibration=x.calibration
		objectivevars=x.objectivevars
		env=x.env
	else:
		xpars=x
		calibrationvars=baseevo.calibrationvars
		calibration=baseevo.calibration
		objectivevars=baseevo.objectivevars
		env=baseevo.env
	
	for index,value in enumerate(xpars):
		correctvalue = max(calibrationvars[index].min,min(calibrationvars[index].max,floor(value/calibrationvars[index].step)*calibrationvars[index].step))
		currentvalue = pycalimero.doublevector()
		currentvalue.append(double(correctvalue))
		calibrationvars[index].setValues(currentvalue)
	
	if error:
		return 0.0
	
	if (not pycalimero.execIteration(calibrationvars)):
		error = True
		return 0.0
	else:
		pycalimero.barrier()
		
	result = 0.0
	resultindex = calibration.getNumOfComplete()-1
	iterationresult = calibration.getIterationResults()[resultindex]
	
	for opar in objectivevars:
		result = result + sum(iterationresult.getResults(opar.getName()))

	return result

def extractData(calibrationvars):
	x = array([],dtype=float32)
	lower = array([],dtype=float32)
	upper = array([],dtype=float32)
	step = array([],dtype=float32)
	
	for parameter in calibrationvars:
		x=append(x,parameter.getInitValues()[0])
		lower=append(lower,parameter.min)
		upper=append(upper,parameter.max)
		step=append(step,parameter.step)
	
	return array([x,lower,upper,step])

class SimpleEvo(Evolvable):
	def __init__(self, x,calibrationvars, objectivevars, env, calibration):
		self.x = array(x[0],dtype=float32)
		self.lower = array(x[1],dtype=float32)
		self.upper = array(x[2],dtype=float32)
		self.step = array(x[3],dtype=float32)
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.env=env
		self.calibration=calibration
	def mutate(self):
		index = numpy.random.random_integers(0,self.x.size-1)
		
		if numpy.random.random() < 0.5:
			self.x[index]=max([self.lower[index], min([self.x[index] + self.step[index], self.upper[index]])])
		else:
			self.x[index]=max([self.lower[index], min([self.x[index] - self.step[index], self.upper[index]])])
	def copy(self):
		return SimpleEvo(array([self.x,self.lower,self.upper,self.step]),self.calibrationvars,self.objectivevars,self.env,self.calibration)      
	def randomize(self):   
		self.x = numpy.maximum(self.lower,numpy.minimum(numpy.floor(((self.upper * numpy.random.random(self.x.size))/self.step))*self.step,self.upper))
	
class Pybrain_HillClimber(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_HillClimber START",pycalimero.Standard)
		
		boundaries = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = HillClimber(objf, boundaries)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_HillClimber DONE",pycalimero.Standard)
        
		return True;

class PyBrain_StochasticHillClimber(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_StochasticHillClimbe START",pycalimero.Standard)
		
		boundaries = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = StochasticHillClimber(objf, boundaries)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_StochasticHillClimbe DONE",pycalimero.Standard)
        
		return True;
	
class PyBrain_RandomSearch(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_RandomSearch START",pycalimero.Standard)
		
		boundaries = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = RandomSearch(objf, boundaries)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_RandomSearch DONE",pycalimero.Standard)
        
		return True;

class PyBrain_CMAES(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
	
	def __del__(self):
		print "delete CMAES" 
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_CMAES START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = CMAES(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_CMAES DONE",pycalimero.Standard)
        
		return True;
		
class PyBrain_NelderMead(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_NelderMead START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = NelderMead(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_NelderMead DONE",pycalimero.Standard)
        
		return True;
		
class PyBrain_OriginalNES(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_OriginalNES START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = OriginalNES(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_OriginalNES DONE",pycalimero.Standard)
        
		return True;
		
class PyBrain_ExactNES(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_ExactNES START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = ExactNES(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_ExactNES DONE",pycalimero.Standard)
        
		return True;
		
class PyBrain_FEM(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_FEM START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = FEM(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_FEM DONE",pycalimero.Standard)
        
		return True;
		
class PyBrain_FiniteDifferences(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_FiniteDifferences START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = FiniteDifferences(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_FiniteDifferences DONE",pycalimero.Standard)
        
		return True;
		
class PyBrain_PGPE(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_PGPE START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = PGPE(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_PGPE DONE",pycalimero.Standard)
        
		return True;
		
class PyBrain_SimpleSPSA(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_SimpleSPSA START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = SimpleSPSA(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_SimpleSPSA DONE",pycalimero.Standard)
        
		return True;
		
class PyBrain_ParticleSwarmOptimizer(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_ParticleSwarmOptimizer START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = ParticleSwarmOptimizer(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_ParticleSwarmOptimizer DONE",pycalimero.Standard)
        
		return True;
		
class PyBrain_GA(pycalimero.ICalibrationAlg):
	def __init__(self):
		pycalimero.ICalibrationAlg.__init__(self)
		self.setDataType("max Iteration", pycalimero.UINT, "100")
		self.setDataType("max Error", pycalimero.DOUBLE, "1")
		self.setDataType("minimize objetive funtion", pycalimero.BOOL, "1")
		self.setDataType("verbose", pycalimero.BOOL, "0")
              
	def start(self,calibrationvars, objectivevars, env, calibration):
		self.calibrationvars=calibrationvars
		self.objectivevars=objectivevars
		self.calibration=calibration
		self.env=env
		global error
		global baseevo 
		error = False
		minimize = False
		verbose = False
		
		if self.getValueOfParameter("minimize objetive funtion")=="1":
			minimize=True
			
		if self.getValueOfParameter("verbose")=="1":
			verbose=True
		
		pycalimero.log("Pybrain_GA START",pycalimero.Standard)
		
		baseevo = SimpleEvo(extractData(self.calibrationvars),calibrationvars,objectivevars,env,calibration)

		alg = GA(objf, baseevo.x)
		alg.maxEvaluations = int(self.getValueOfParameter("max Iteration"))
		alg.minimize= minimize
		alg.desiredEvaluation = float(self.getValueOfParameter("max Error"))
		alg.verbose = verbose
		alg.learn()

		pycalimero.log("Pybrain_GA DONE",pycalimero.Standard)
        
		return True;
        


			
		
