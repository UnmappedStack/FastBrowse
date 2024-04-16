# FastBrowse
A faster Windows file explorer alternative.

## Introduction
Windows file explorer is incredibly slow. For this reason, I've been developing an open-source, fast, and (in my opinion), stylish new file manager called FastBrowse. The project is still very much in progress, so no code or releases have been published yet. However, good progress has been made. The following is a screenshot of a test version of FastBrowse in it's current stage:

![Screenshot of a test version of FastBrowse](https://github.com/jakeSteinburger/FastBrowse/blob/main/test_screenshot.png)

## Inner workings
The backend is written completely in C++, making use of a few open source libraries listed in CREDITS.md. The front-end is written in JavaScript and formatted with HTML and CSS. The backend communicates with the frontend through a local HTTP server on port 1104, meaning that port 1104 *must* be avaliable the whole time that FastBrowse is installed on the client device. This project uses the Windows API for various purposes, meaning that at the moment, FastBrowse will be *only avaliable on Windows*.

FastBrowse is split into 3 seperate applications:
* **cacheFiles.exe** regularly crawls the whole device for files, and stores them all in a single file, keeping the application's knowledge of the file system up-to-date. This is a constantly running background application.
* **runServer.exe** runs a local HTTP server on the client device which the front-end can make API calls to. This is always running in the background.
* **FastBrowse.exe** is the application which runs the frontend. It runs an embedded copy of the Chromium web browser, which holds the JavaScript which displays the GUI. This can make API calls to runServer.exe.
