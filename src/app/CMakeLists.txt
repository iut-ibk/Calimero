FILE(GLOB GUI_CPP *.cpp)
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

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(calimero ${GUI_CPP})

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        TARGET_LINK_LIBRARIES(calimero calimerocore pycalimero calimerohelper ${QT_QTGUI_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${QT_QTMAIN_LIBRARY})
ELSE()
        TARGET_LINK_LIBRARIES(calimero calimerocore pycalimero calimerohelper ${QT_QTMAIN_LIBRARY})
ENDIF()

INSTALL(TARGETS calimero RUNTIME DESTINATION bin)
