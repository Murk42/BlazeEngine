:: %1 - solution directory
:: %2 - project directory
:: %3 - configuration
:: %4 - platform target
echo .
echo Copying include files
xcopy /s /e /i /f /y /d %2include %1install\include
echo .
echo Copying build files:
xcopy /s /e /i /f /y /d %2build\%3\%4 %1install\lib
echo .
echo Copying dependencies:
xcopy /s /e /i /f /y /d %1external\%3\bin %1\install\bin