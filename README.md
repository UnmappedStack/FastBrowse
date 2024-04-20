# FastBrowse
A faster Windows file explorer alternative.

## Introduction
Windows file explorer is incredibly slow. For this reason, I've been developing an open-source, fast, and (in my opinion), stylish new file manager called FastBrowse. The project is still very much in progress, so there are various issues that I'm aware of and plan to fix in the near future. However, good progress has been made. The following is a screenshot of FastBrowse in it's current stage:

![Screenshot of FastBrowse](https://github.com/jakeSteinburger/FastBrowse/blob/main/fastbrowsescreenshot.png)

## Inner workings
The backend is written completely in C++, making use of a few open source libraries listed in CREDITS.md. The front-end is written in JavaScript and formatted with HTML and CSS. The backend communicates with the frontend through a local HTTP server on port 1104, meaning that port 1104 *must* be avaliable the whole time that FastBrowse is installed on the client device. This project uses the Windows API for various purposes, meaning that at the moment, FastBrowse will be *only avaliable on Windows*.

FastBrowse is split into 3 seperate applications:
* **FastBrowseBackgroundCache.exe** regularly crawls the whole device for files, and stores them all in a single file, keeping the application's knowledge of the file system up-to-date. This is a constantly running background application.
* **FastBrowseBackgroundServer.exe** runs a local HTTP server on the client device which the front-end can make API calls to. This is always running in the background.
* **fastbrowse.exe** is the application which runs the frontend. It runs an embedded copy of the Chromium web browser using Electron, which holds the JavaScript which displays the GUI. This can make API calls to FastBrowseBackgroundServer.exe.

## Issues
There are a some known issues with FastBrowse, such as file renaming not working properly. If you find an issue, please report it in the issues tab. I am actively working on fixing these issues.

## Installation
Currently I am working on an installer for FastBrowse (you can see there's a bin folder in the repo for that), but until then, you'll need to build from source yourself. Here are the steps:

### Prerequisites
* You'll need to first have NodeJS installed for the frontend to work. You can run install NodeJS from [the official NodeJS website](https://nodejs.org/en/download/)
* Before you can begin, clone the repo with:
```
git clone https://github.com/jakeSteinburger/FastBrowse.git
```
* You'll also need `cpp-httplib` before you can start. Clone the repo anywhere on your device with:
```
git clone https://github.com/yhirose/cpp-httplib.git
```
* Finally, you'll need Visual Studio 2022, which you can download from the Microsoft store.
### Setup
Open the `frontend` directory in the repo, then make sure that electron is installed as a dependency:
```
cd FastBrowse/frontend
npm install --save-dev electron
```
### Building
#### Bulding the frontend
Begin by building the frontend. Import Electron Forge into the repo:
```
npm install --save-dev @electron-forge/cli
npx electron-forge import
```
Then compile into an executable:
```
npm run make
```
You can find `fastbrowse.exe` in the directory `out/make` directory, which you can create a shortcut `.lnk` to anywhere you need. However, the application is not usable yet because you still need the backend for the program to communicate with your file system.
#### Building the backend
Now you can build the backend. First, build the background server application. Navigate to the root directory of the FastBrowse repository and open the FastBrowseBackgroundCache folder, then open FastBrowseBackgroundServer.sln in Visual Studio. In the menu, click `Project` then `FastBrowseBackgroundServerProperties` at the bottom. In the `C/C++` section on the right, navigate to general and find the setting for `Include Additional Directories`, and add the directory of the project as well as the directory of `cpp-httplib`. Now, in `Configuration Properties` > `General` on the left, find `C++ Language Standard` and make sure it's set to `ISO C++20 Standard`. After making these changes, click Apply, close the Properties window and press Control + Shift + B. Wait for it to compile and find the folder `x64/Debug`. Move this file to the root FastBrowse repo directory.

Now repeat these steps with `FastBrowseBackgroundCache`. Now that you have both executables in the root FastBrowse repository, you can run `addToStartup.bat` to automatically add these background applications to run in the background when your device turns on. This is required for the program to function. After it's finished running, restart your device and wait about five minutes for the cache to initialise.

Now, you can run the application from `fastbrowse.exe`.

## Sorry
Please note that this was quite a learning project for me as I got used to C++ rather than more simple languages, so I'm aware that this isn't the most efficient, and I'm also aware the build instructions are quite annoying. I also do know that I probably haven't used the best practices for making my code readable, and there are large portions of commented out code - so to anybody trying to make changes... **I'm sorry.**