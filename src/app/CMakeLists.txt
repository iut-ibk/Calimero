FILE(GLOB GUI_CPP *.cpp *.h)
FILE(GLOB GUI_UI *.ui)

QT4_WRAP_UI(GUI_CPP ${GUI_UI})
SET(MOC_HEADERS mainwindow.h
                guilogsink.h
                parametertextedit.h
                groupmanager.h
                stringparameteredit.h
                diagramgui.h
                resultanalysisdialog.h
                settingsdialog.h)
QT4_WRAP_CPP(GUI_CPP ${MOC_HEADERS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET_SOURCE_FILES_PROPERTIES(${CMAKE_BINARY_DIR}/pycalimeroWRAP.cpp PROPERTIES GENERATED 1)
SET(GUI_CPP ${GUI_CPP} ../python-swig/PyFunctionLoader.cpp ../python-swig/PyEnv.cpp ${CMAKE_BINARY_DIR}/pycalimeroWRAP.cpp)

ADD_EXECUTABLE(calimero ${GUI_CPP})

ADD_DEPENDENCIES(calimero pycalimerogen)

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        TARGET_LINK_LIBRARIES(calimero calimerocore calimerohelper ${QT_QTGUI_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${QT_QTMAIN_LIBRARY})
ELSE()
        TARGET_LINK_LIBRARIES(calimero calimerocore calimerohelper ${QT_QTMAIN_LIBRARY})
ENDIF()

INSTALL(TARGETS calimero DESTINATION bin
PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
