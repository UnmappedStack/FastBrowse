@echo off

set "source1=FastBrowseBackgroundCache.exe"
set "source2=FastBrowseBackgroundServer.exe"

set "shortcut1=%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup\fbcache.lnk"
set "shortcut2=%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup\fbserver.lnk"

echo Creating shortcut for background cache executable in the startup folder...
echo [InternetShortcut] >"%shortcut1%"
echo URL=file:///%source1% >>"%shortcut1%"

echo Creating shortcut for background server executable in the startup folder...
echo [InternetShortcut] >"%shortcut2%"
echo URL=file:///%source2% >>"%shortcut2%"

echo Done. Please restart your device so that the required background modules can run in the background.
pause
