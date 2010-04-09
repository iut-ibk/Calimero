class MyOF(calimerocore.ObjectiveFunctionInterface):
	def __init__(self):
		print "python hallo new"
		calimerocore.ObjectiveFunctionInterface.__init__(self)

	def eval(self):
		print "fein eval called in %s" % self.__class__.__name__

	def __del__(self):
		print "python delete"


class MyOF1(calimerocore.ObjectiveFunctionInterface):
	def __init__(self):
		calimerocore.ObjectiveFunctionInterface.__init__(self)

	def eval(self):
		print "eval called in %s" % self.__class__.__name__

	def __del__(self):
		print "python delete"

class Vector(object):
	def __init__(self, x, y):
		self.x = x
		self.y = y

	def staticf():
		pass

	def __add__(self, other):
		print "add called"

	def __str__(self):
		return "(%s, %s)" % (self.x, self.y)

	def __call__(self):
		print "called"

	def __del__(self):
		print "deleted"

#for c in calimerocore.ObjectiveFunctionInterface.__subclasses__():
#	cinst = c()
#	cinst.eval()
#	del cinst

#x = calimerocore.doublev()
#for i in range(100): 
#	x.append(i) 
#calimerocore.testdoublev(x)
#print "ende"

