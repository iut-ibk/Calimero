import pycalimero
import calimeropublic
import sys, os, random
from PyQt4.QtCore import *
from PyQt4.QtGui import *
import array
import matplotlib.patches as patches
import matplotlib.path as path
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt

import matplotlib
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt4agg import NavigationToolbar2QTAgg as NavigationToolbar
from mpl_toolkits.axes_grid.axislines import SubplotZero
from matplotlib.figure import Figure
import numpy as np
import math
from calimeropublic import findBestIterationNumber
from matplotlib.pyplot import boxplot

class Dialog(QDialog):
    def __init__(self,parent=None):
        QDialog.__init__(self, parent)
        self.resize(600,600)
        self.setWindowTitle('')
        self.create_main_frame()
    
    def showFigure(self, figure):
        self.fig= figure
        self.canvas = FigureCanvas(self.fig)
        self.canvas.setParent(self.main_frame)
        self.mpl_toolbar = NavigationToolbar(self.canvas, self)
        hbox = QHBoxLayout()
        vbox = QVBoxLayout()
        vbox.addWidget(self.canvas)
        vbox.addWidget(self.mpl_toolbar)
        vbox.addLayout(hbox)
        self.main_frame.setLayout(vbox)
        self.canvas.draw()
        self.show()
    
    def create_main_frame(self):
        self.main_frame = QWidget()
        mainbox = QVBoxLayout()
        mainbox.addWidget(self.main_frame)
        self.setLayout(mainbox)
    
    def add_actions(self, target, actions):
        for action in actions:
            if action is None:
                target.addSeparator()
            else:
                target.addAction(action)
    
    def create_action(  self, text, slot=None, shortcut=None, 
                        icon=None, tip=None, checkable=False, 
                        signal="triggered()"):
        action = QAction(text, self)
        if icon is not None:
            action.setIcon(QIcon(":/%s.png" % icon))
        if shortcut is not None:
            action.setShortcut(shortcut)
        if tip is not None:
            action.setToolTip(tip)
            action.setStatusTip(tip)
        if slot is not None:
            self.connect(action, SIGNAL(signal), slot)
        if checkable:
            action.setCheckable(True)
        return action

class SMDotPlot(pycalimero.IResultHandler):
    def __init__(self):
        pycalimero.IResultHandler.__init__(self)
        self.setDataType("parameter 1", pycalimero.STRING, "")
        self.setDataType("parameter 2", pycalimero.STRING, "")
        self.setDataType("title", pycalimero.STRING, "")
        self.setDataType("iteration number", pycalimero.UINT, "0")
        
    def run(self, results):
        par1 = self.getValueOfParameter("parameter 1")
        par2 = self.getValueOfParameter("parameter 2")
        i = int(self.getValueOfParameter("iteration number"))
        title = self.getValueOfParameter("title")
        
        if(par1==""):
            return False
        
        if(par2==""):
            return False
        
        if(i >= results.__len__()):
            return False
        
        
        dialogform = Dialog(QApplication.activeWindow())
        fig = Figure((5.0, 4.0), dpi=100)
        ax = SubplotZero(fig, 1, 1, 1)
        fig.add_subplot(ax)

        for n in ["top", "right"]:
            ax.axis[n].set_visible(False)
            
        for n in ["bottom", "left"]:
            ax.axis[n].set_visible(True)
        
        x = results[i].getResults(par1)
        y = results[i].getResults(par2)    
        
        if(not(x.__len__())):
            return False
        
        if(not(y.__len__())):
            return False
        
        ax.plot(x,y,'.')
        
        #plot middle
        xm = range(math.floor(min(ax.axis())),math.floor(max(ax.axis())+1),1)
        ax.plot(xm,xm)
        ax.set_xlabel(par1)
        ax.set_ylabel(par2)
        ax.set_title(title)
        dialogform.showFigure(fig)
        return True
    
class FindBestResult(pycalimero.IResultHandler):
    def __init__(self):
        pycalimero.IResultHandler.__init__(self)
        self.setDataType("objective function parameter", pycalimero.STRING, "")
        
    def run(self, results):
        name = self.getValueOfParameter("objective function parameter")
        iteration = findBestIterationNumber(results,name)
        bestvalue = -1
        if(iteration > -1):
            bestvalue = results[0].getObjectiveFunctionParameterResults(name)[0]
        
        title = "Best result"
        values = (name, str(iteration), str(bestvalue) )
        text = "Name: %s\nIteration: %s\n Value: %s" % values
        QMessageBox.information(QApplication.activeWindow(),title,text)
        return True

class GetCalibrationParameters(pycalimero.IResultHandler):
    def __init__(self):
        pycalimero.IResultHandler.__init__(self)
        self.setDataType("Iteration", pycalimero.UINT, "0")
        
    def run(self, results):
        iteration = int(self.getValueOfParameter("Iteration"))
        if results.__len__() <= iteration:
            QMessageBox.information(QApplication.activeWindow(),"Error","There are no results for iteration %s" % iteration) 
            return False
        
        names = results[iteration].getNamesOfCalibrationParameters()
        title = "Calibration values of iteration %s" % iteration
        messagetext = ""
        
        for name in names:
            value = results[iteration].getCalibrationParameterResults(name)[0]
            values = (messagetext , name , value )
            messagetext =  "%sName: %s Value: %f\n" % values
                                                                 
        QMessageBox.information(QApplication.activeWindow(),title,messagetext)                                                       
        return True

class FindAllBestResults(pycalimero.IResultHandler):
    def __init__(self):
        pycalimero.IResultHandler.__init__(self)
        
    def run(self, results):
        if(not(results.__len__())):
            return False
        
        names = results[0].getNamesOfObjectiveFunctionParameters()
        title = "Best results"
        messagetext = ""
        
        for name in names:
            iteration = findBestIterationNumber(results,name)
            bestvalue = -1
            if(iteration > -1):
                bestvalue = results[iteration].getObjectiveFunctionParameterResults(name)[0]
            
            values = (messagetext, name, str(iteration), bestvalue )
            messagetext =  "%sName: %s Iteration: %s Value: %f\n" % values
                                                                 
        QMessageBox.information(QApplication.activeWindow(),title,messagetext)                                                       
        return True
    
class HistogramPlot(pycalimero.IResultHandler):
    def __init__(self):
        pycalimero.IResultHandler.__init__(self)
        self.setDataType("parameter", pycalimero.STRING, "")
        self.setDataType("title", pycalimero.STRING, "")
        self.setDataType("iteration number", pycalimero.UINT, "0")
        
    def run(self, results):
        par = self.getValueOfParameter("parameter")
        i = int(self.getValueOfParameter("iteration number"))
        title = self.getValueOfParameter("title")
        if(par==""):
            return False
        
        if(i >= results.__len__()):
            return False
        
        dialogform = Dialog(QApplication.activeWindow())
        fig = Figure((5.0, 4.0), dpi=100)
        ax = SubplotZero(fig, 1, 1, 1)
        fig.add_subplot(ax)

        for n in ["top", "right"]:
            ax.axis[n].set_visible(False)
            
        for n in ["bottom", "left"]:
            ax.axis[n].set_visible(True)
        
        x = results[i].getResults(par)   
        
        if(not(x.__len__())):
            return False

        ax.hist(x, 100, normed=1)
        ax.set_xlabel('Error')
        ax.set_ylabel('Probability')
        ax.grid(True)
        
        ax.set_title(title)
        dialogform.showFigure(fig)
        return True
    
class BoxPlot(pycalimero.IResultHandler):
    def __init__(self):
        pycalimero.IResultHandler.__init__(self)
        self.setDataType("parameter", pycalimero.STRING, "")
        self.setDataType("title", pycalimero.STRING, "")
        self.setDataType("iteration number", pycalimero.UINT, "0")
        
    def run(self, results):
        par = self.getValueOfParameter("parameter")
        i = int(self.getValueOfParameter("iteration number"))
        title = self.getValueOfParameter("title")

        if(par==""):
            return False
        
        if(i >= results.__len__()):
            return False
        
        dialogform = Dialog(QApplication.activeWindow())
        fig = Figure((5.0, 4.0), dpi=100)
        ax = SubplotZero(fig, 1, 1, 1)
        fig.add_subplot(ax)

        for n in ["top", "right"]:
            ax.axis[n].set_visible(False)
            
        for n in ["bottom", "left"]:
            ax.axis[n].set_visible(True)
        
        x = results[i].getResults(par)   
        
        if(not(x.__len__())):
            return False
        
        ax.boxplot(x, notch=0, sym='+', vert=1, whis=1.5)
        
        ax.set_title(title)
        dialogform.showFigure(fig)
        return True
    
class TimeVariationCurvePlot(pycalimero.IResultHandler):
    def __init__(self):
        pycalimero.IResultHandler.__init__(self)
        self.setDataType("parameter 1", pycalimero.STRING, "")
        self.setDataType("parameter 2", pycalimero.STRING, "")
        self.setDataType("title", pycalimero.STRING, "")
        self.setDataType("iteration number", pycalimero.UINT, "0")
        
    def run(self, results):
        par1 = self.getValueOfParameter("parameter 1")
        par2 = self.getValueOfParameter("parameter 2")
        i = int(self.getValueOfParameter("iteration number"))
        title = self.getValueOfParameter("title")
        
        if(par1==""):
            return False
        
        if(par2==""):
            return False
        
        if(i >= results.__len__()):
            return False
        
        
        dialogform = Dialog(QApplication.activeWindow())
        fig = Figure((5.0, 4.0), dpi=100)
        ax = SubplotZero(fig, 1, 1, 1)
        fig.add_subplot(ax)

        for n in ["top", "right"]:
            ax.axis[n].set_visible(False)
            
        for n in ["bottom", "left"]:
            ax.axis[n].set_visible(True)
        
        y1 = results[i].getResults(par1)
        y2 = results[i].getResults(par2)    
        
        if(not(y1.__len__())):
            return False
        
        if(not(y2.__len__())):
            return False
        
        ax.plot(range(0,y1.__len__()),y1,color='r')
        ax.plot(range(0,y2.__len__()),y2,color='b')
        ax.set_title(title)
        
        leg = ax.legend((par1, par2),
           'upper center', shadow=True)
        
        frame  = leg.get_frame()
        frame.set_facecolor('0.80')    # set the frame face color to light gray
        
        # matplotlib.text.Text instances
        for t in leg.get_texts():
            t.set_fontsize('small')    # the legend text fontsize
        
        # matplotlib.lines.Line2D instances
        for l in leg.get_lines():
            l.set_linewidth(1.5)  # the legend line width
        
        dialogform.showFigure(fig)
        return True
