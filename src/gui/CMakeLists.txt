FILE(GLOB GUI_CPP *.cpp)
FILE(GLOB GUI_UI *.ui)

QT4_WRAP_UI(GUI_CPP ${GUI_UI})
SET(MOC_HEADERS mainwindow.h
                guilogsink.h
                parametertextedit.h)
QT4_WRAP_CPP(GUI_CPP ${MOC_HEADERS})
#QT4_ADD_RESOURCES(GUI_CPP ../../data/resources/calimero-gui.qrc)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(calimero-gui WIN32 ${GUI_CPP}) # ../app/saxloader.cpp)
TARGET_LINK_LIBRARIES(calimero-gui calimerocore pycalimero calimerohelper ${QT_QTGUI_LIBRARY} ${QT_QTMAIN_LIBRARY})

INSTALL(TARGETS calimero-gui RUNTIME DESTINATION bin)
