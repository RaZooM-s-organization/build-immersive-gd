@echo off
setlocal enabledelayedexpansion


REM *VIBECODED!!! - no idea how to write .cmd programs

REM Use geode secret functionality of adding dll-s as resources:
REM 1) unzip .geode file
REM 2) add .dll-s to resources/binaries/win
REM 3) zip back

REM Check if argument is provided
if "%~1"=="" (
    echo Error: No file specified!
    echo Usage: %~nx0 ^<filename^>
    exit /b 1
)

REM Get first argument - filename
set "filename=%~1"
set "basename=%~n1"
set "dirname=%~dp1"
echo Processing file: %filename%

REM Check if file exists
if not exist "%filename%" (
    echo File not found: %filename%
    exit /b 2
)

echo File exists: %filename%

REM Rename file to .zip extension
echo Renaming file to %filename%.zip...
move "%filename%" "%filename%.zip"
if !errorlevel! neq 0 (
    echo Error renaming file
    exit /b 3
)
echo File successfully renamed to %filename%.zip

REM Extract the archive
echo Extracting %filename%.zip...
REM Create temporary directory for extraction
set "temp_dir=temp_%basename%"
mkdir "!temp_dir!" 2>nul
powershell -Command "Expand-Archive -Path '%filename%.zip' -DestinationPath '!temp_dir!' -Force"
if !errorlevel! neq 0 (
    echo Error extracting file
    rmdir /s /q "!temp_dir!" 2>nul
    exit /b 4
)
echo File successfully extracted

REM Determine extracted folder name (use temp directory)
set "folder_name=!temp_dir!"

REM Create resources/binaries/win path in extracted folder
echo Creating resources/binaries/win path in folder !folder_name!...
if not exist "!folder_name!" (
    echo Folder !folder_name! not found after extraction
    exit /b 6
)

mkdir "!folder_name!\resources\binaries\win" 2>nul
if not exist "!folder_name!\resources\binaries\win" (
    echo Error creating path in !folder_name!
    exit /b 5
)
echo Path resources/binaries/win successfully created in !folder_name!

REM Copy all .dll files from ./lib/ffmpeg-master-latest-win64-gpl-shared/bin to created path
echo Copying .dll files from ./lib/ffmpeg-master-latest-win64-gpl-shared/bin to !folder_name!\resources\binaries\win...
if not exist "lib\ffmpeg-master-latest-win64-gpl-shared\bin\*.dll" (
    echo No .dll files found in lib\ffmpeg-master-latest-win64-gpl-shared\bin folder
    exit /b 8
)

copy "lib\ffmpeg-master-latest-win64-gpl-shared\bin\*.dll" "!folder_name!\resources\binaries\win\" /Y >nul
if !errorlevel! neq 0 (
    echo Error copying .dll files
    exit /b 7
)
echo .dll files successfully copied

REM Archive folder back
echo Archiving folder !folder_name! back to ZIP...
powershell -Command "Compress-Archive -Path '!folder_name!\*' -DestinationPath '!folder_name!_new.zip' -Force"
if !errorlevel! neq 0 (
    echo Error archiving folder
    exit /b 10
)
echo Folder successfully archived to !folder_name!_new.zip

REM Replace original archive with new one
echo Replacing original archive with new one...
del "%filename%.zip" 2>nul
move "!folder_name!_new.zip" "%filename%"
if !errorlevel! neq 0 (
    echo Error replacing archive
    exit /b 9
)
echo Original archive successfully replaced

REM Clean up temporary folder
echo Removing temporary folder !folder_name!...
rmdir /s /q "!folder_name!" 2>nul

echo Processing completed successfully!
