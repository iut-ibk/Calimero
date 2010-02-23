cmake_minimum_required(VERSION 2.6)

find_package(Qt4 COMPONENTS QtCore QtGui QtXml QtScript QtScriptTools REQUIRED)
find_library(QT_QTSCRIPTTOOLS_LIBRARY NAMES QtScriptTools PATHS ${QT_LIBRARY_DIR} )
find_path(QT_SCRIPTTOOLS_INCLUDE_DIR NAMES qscriptenginedebugger.h PATHS ${QT_INCLUDE_DIR}/QtScriptTools )
include_directories(${QT_INCLUDES} ${QT_SCRIPTTOOLS_INCLUDE_DIR})

set(QT_USE_QTXML 1)
set(QT_USE_QTSCRIPT 1)
set(QT_USE_QTSVG 1)
set(QT_USE_QTSCRIPT_TOOLS 1)

include(${QT_USE_FILE})
INCLUDE_DIRECTORIES(	${CMAKE_CURRENT_SOURCE_DIR}/persistence 
			${CMAKE_CURRENT_SOURCE_DIR}/gui 
			${CMAKE_CURRENT_SOURCE_DIR}/algs 
			${CMAKE_CURRENT_SOURCE_DIR}/algs/ga 
			${CMAKE_CURRENT_SOURCE_DIR}/core 
			${CMAKE_CURRENT_SOURCE_DIR}/qtscript 
			${CMAKE_CURRENT_SOURCE_DIR}
			${calimero_BINARY_DIR}/src/gui
			${calimero_BINARY_DIR}/src
)

SET(CALIMERO_SRCS 	main.cpp
			algs/Algorithms.cpp 
			gui/CalibrationEditGui.cpp       
			gui/Gui.cpp                    
			gui/ParameterTextEdit.cpp
			gui/CompareParameterEditGui.cpp 
			gui/ParameterChooserGui.cpp    
			gui/ProgressBar.cpp
			gui/Controller.cpp               
			gui/ParameterEditGui.cpp       
			gui/SettingsDialog.cpp
			gui/DiagramGui.cpp               
			gui/ParameterFilesGui.cpp
			gui/Diagramscene.cpp             
			gui/ParameterGroupEditGui.cpp
			qtscript/qscripthighlighter.cpp  
			qtscript/scripteditwidget.cpp  
			qtscript/ScriptLibary.cpp
			core/CalculationVariable.cpp  
			core/FileLoader.cpp         
			core/Language.cpp
			core/CalibrationVariable.cpp  
			core/IterationResult.cpp    
			core/Model.cpp
			core/CoreEngine.cpp           
			core/IterationRunnable.cpp  
			core/Variable.cpp
			persistence/ParameterPersistenceEvaluator.cpp  
			persistence/Persistence.cpp
			algs/ga/Gaalg.cpp 
)

SET(CALIMERO_MOC_H	gui/DiagramGui.h
			core/CalculationVariable.h     
			gui/Gui.h
			core/CalibrationVariable.h     
			gui/ParameterChooserGui.h
			core/CoreEngine.h              
			gui/ParameterEditGui.h
			gui/ParameterFilesGui.h
			core/IterationResult.h         
			gui/ParameterGroupEditGui.h
			gui/ParameterTextEdit.h
			gui/ProgressBar.h
			gui/SettingsDialog.h
			core/Model.h                   
       			core/Variable.h                
			qtscript/qscripthighlighter.h
			gui/CalibrationEditGui.h       
			qtscript/scripteditwidget.h
			gui/CompareParameterEditGui.h 
			gui/Controller.h
)

SET(CALIMERO_UIS	gui/mainwindow.ui
			gui/calibrationedit.ui         
			gui/parameterfiles.ui
			gui/compareparameteredit.ui  
			gui/parameterchooser.ui  
			gui/progressbar.ui
			gui/inputparameteredit.ui    
			gui/parameteredit.ui     
			gui/settingsdialog.ui
			qtscript/scripteditwidget.ui
)

QT4_WRAP_UI(CALIMERO_UIS_H ${CALIMERO_UIS})
QT4_WRAP_CPP(CALIMERO_MOC_SRCS ${CALIMERO_MOC_H})

add_executable(calimero ${CALIMERO_SRCS} ${CALIMERO_UIS_H} ${CALIMERO_MOC_SRCS})
target_link_libraries(calimero ${QT_LIBRARIES} ${QT_QTSCRIPTTOOLS_LIBRARY})

IF(WIN32 AND NOT UNIX)
	INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/calimero.exe" DESTINATION bin)
ELSE(WIN32 AND NOT UNIX)
	INSTALL(PROGRAMS "${CMAKE_CURRENT_BINARY_DIR}/calimero" DESTINATION bin)	 
ENDIF(WIN32 AND NOT UNIX)


FILE(GLOB CALIBRATIONSCRIPTS "${calimero_SOURCE_DIR}/testdaten/calimerohome/calibrationscripts/default/*.qs")
INSTALL(FILES ${CALIBRATIONSCRIPTS} DESTINATION calimerohome/calibrationscripts/default)
FILE(GLOB COMPARESCRIPTS "${calimero_SOURCE_DIR}/testdaten/calimerohome/comparescripts/default/*.qs")
INSTALL(FILES ${COMPARESCRIPTS} DESTINATION calimerohome/comparescripts/default)
FILE(GLOB LANGUAGE "${calimero_SOURCE_DIR}/src/*.qm")
INSTALL(FILES ${LANGUAGE} DESTINATION calimerohome/language)

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Calimero")
SET(CPACK_PACKAGE_VENDOR "Michael Mair")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "0")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "calimero_${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
IF(WIN32 AND NOT UNIX)
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  SET(CPACK_NSIS_CONTACT "michael.mair@uibk.ac.at")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_GENERATOR "DEB")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Michael Mair <michael.mair@uibk.ac.at>")	 
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "Calimero" "Calimero")
INCLUDE(CPack)
