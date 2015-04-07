# VIP
Visiology Intelligence Platform

#################### Build requirements ######################

1) VS2013 (services\Центр Разработки\Flipbox\!dev\__Soft\Visual Studio\VS2013\)
2) Qt 5.4.1 (services\Центр Разработки\Flipbox\!dev\__Soft\Qt\qt-opensource-windows-x86-msvc2013-5.4.1.exe)
3) R language (services\Центр Разработки\Flipbox\!dev\__Soft\R-3.1.3-win.exe)

###################### Build procedure #######################

1) Add R to system path
   Ex: (R installed in c:\R-3.1.3\, add following line to your path:
        c:\R-3.1.3\bin\i386\
        )
2) Copy headers from dep/include/R to $$R_HOME/include

########################### Run ##############################

* REditor
  Make sure that R is included to your path (see Build procedure step 1)
  Make sure that Qt is included to your path

* Tests
  Make sure that Qt is included to your path