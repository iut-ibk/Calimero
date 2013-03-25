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
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} )

SET(GUI_CPP ${GUI_CPP})

ADD_EXECUTABLE(calimero ${GUI_CPP})

TARGET_LINK_LIBRARIES(calimero ${Boost_LIBRARIES} ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY} calimerocore calimerohelper calimeropythonenv)

INSTALL(TARGETS calimero DESTINATION bin
PERMISSIONS OWNER_READ OWNER_EXECUTE OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
