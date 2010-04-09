import sys
sys.path.append("/home/csae6550/Desktop/calimerocoredev/build/Debug/calimerocore")

import calimerocore

class MyOF(calimerocore.ObjectiveFunctionInterface):
	def __init__(self):
		print "python hallo"
		calimerocore.ObjectiveFunctionInterface.__init__(self)

	def eval(self):
		print "eval called in %s" % self.__class__.__name__

	def __del__(self):
		print "python delete"


def load(modulename):

