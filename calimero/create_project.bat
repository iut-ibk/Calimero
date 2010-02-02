qmake -project
echo CONFIG += debug >> calimero.pro
echo CONFIG += qt >> calimero.pro
echo QT += gui >> calimero.pro
echo QT += xml >> calimero.pro
echo QT += script >> calimero.pro
qmake -tp vc
qmake
nmake clean
nmake release