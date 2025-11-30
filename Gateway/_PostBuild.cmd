set SOLUTION_DIR=%1
set OUT_DIR=%2

rem We copy mesh-config to the output folder...
copy "%SOLUTION_DIR%Solution Items\mesh-config.json" "%OUT_DIR%"

