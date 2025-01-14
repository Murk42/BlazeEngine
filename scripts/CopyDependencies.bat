:: %1 - blaze soulution folder
:: %2 - destination folder
:: %3 - configuration
:: %4 - platform target

echo Copying dependencies:
xcopy /s /e /i /r /f /y /d %1BlazeEngineCore\build\%3\%4\*.dll %2
xcopy /s /e /i /r /f /y /d %1BlazeEngine\build\%3\%4\*.dll %2
xcopy /s /e /i /r /f /y /d %1BlazeEngineGraphics\build\%3\%4\*.dll %2
xcopy /s /e /i /r /f /y /d %1BlazeEngineGraphics\assets %2