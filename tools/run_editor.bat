@echo off
set PYTHON=C:\Users\naike\AppData\Local\Programs\Python\Python312\python.exe

if not exist "%PYTHON%" (
    set PYTHON=C:\Users\lenovo\.codegeex\mamba\envs\codegeex-agent\python.exe
)

if not exist "%PYTHON%" (
    echo Python not found. Please install Python 3 with Tkinter.
    echo Or edit this bat file to point to your Python installation.
    pause
    exit /b 1
)

cd /d "%~dp0\.."

"%PYTHON%" "%~dp0\ocmp_map_editor.py" %*

pause
