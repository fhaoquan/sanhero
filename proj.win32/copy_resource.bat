if not exist "%1" mkdir "%1"
xcopy "..\Resources" "%1" /D /E /I /F /Y
      