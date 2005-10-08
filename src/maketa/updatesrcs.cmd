echo "Copying src/ta files into src/maketa..."
@echo off
xcopy "%CD%"\..\ta\ta_stdef.h "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\platform.h "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\ta_string.h "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\ta_list.h "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\ta_type.h "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\ta_ti.cpp "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\ta_platform.cpp "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\ta_platform_win.cpp "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\ta_string.cpp "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\ta_list.cpp "%CD%" /D /Y /i /q
xcopy "%CD%"\..\ta\ta_type.cpp "%CD%" /D /Y /i /q
@echo on
