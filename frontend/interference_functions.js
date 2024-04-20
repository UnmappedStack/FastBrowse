                let currentDirectory = "";
                let lastBackFrom = [];
                let clipboardFiles = [];
                let clipboardIsCut = false;
                let iswholedevicesearch = false;

                function chooseIcon(filename) {
                    const imageFileTypes = ["jpg", "jpeg", "png", "gif", "bmp", "tiff", "svg", "webp", "ico"];
                    const videoFileTypes = ["mp4", "mov", "avi", "mkv", "wmv", "flv", "webm", "m4v", "mpg", "mpeg"];
                    const codeFileExtensions = ["js","html","css","java","py","rb","cpp","c","php","swift","ts","dart","go","rust","lua","perl","sh","r","jsx","tsx","scss","sass","less","sql","asm","json","yaml","xml","svg","yml","md","htaccess","conf"];
                    const archiveFileTypes = ["zip", "rar", "7z", "tar", "gz", "bz2"];
                    const documentFileTypes = ["doc", "docx", "ppt", "pptx", "xls", "xlsx", "pdf"];
                    const executableFileTypes = ["exe", "msi"];
                    const audioFileTypes = ["mp3", "wav", "ogg", "flac", "aac"];
                    let ext = filename.split(".").pop().toLowerCase();
                    if (imageFileTypes.includes(ext)) {
                        return "<i class='fa fa-file-image-o'></i>";
                    } else if (videoFileTypes.includes(ext)) {
                        return "<i class='fa fa-file-video-o'></i>";
                    } else if (codeFileExtensions.includes(ext)) {
                        return "<i class='fa fa-file-code-o'></i>";
                    } else if (archiveFileTypes.includes(ext)) {
                        return "<i class='fa fa-file-archive-o'></i>";
                    } else if (documentFileTypes.includes(ext)) {
                        return "<i class='fa fa-file-word-o'></i>";
                    } else if (executableFileTypes.includes(ext)) {
                        return "<i class='fa fa-laptop'></i>";
                    } else if (audioFileTypes.includes(ext)) {
                        return "<i class='fa fa-file-audio-o'></i>";
                    } else {
                        return "<i class='fa fa-file-o'></i>";
                    }
                }

                
                function displayFiles(response, issearch = false) {
                    document.getElementById("filelist").innerHTML = "";
                    let splitFiles = response.split("[|filesplit|]");
                    for (let i = 0; i < splitFiles.length; i++) {
                        // Check folder or file
                        let lastChar = splitFiles[i].charAt(splitFiles[i].length - 1);
                        let toAdd = "";
                        if (issearch) {
                            splitFiles[i] = currentDirectory + splitFiles[i];
                        }
                        if (lastChar == "\\") {
                            splitFilesOriginal = splitFiles[i];
                            splitFiles[i] = splitFiles[i].split("\\");
                            splitFiles[i] = splitFiles[i][splitFiles[i].length - 2];
                            toAddtxt = "openFolder('" + splitFilesOriginal + "');";
                            toAddtxt = toAddtxt.replaceAll("\\", "\\\\");
                            if (iswholedevicesearch) {
                                splitFiles[i] += "<small style='color: grey;'>&nbsp; " + splitFilesOriginal + "</small>";
                            }
                            toAdd = '<div class="file" onclick="' + toAddtxt + '"><input type="checkbox" onclick="event.stopPropagation();"> <i style="color: #FFC84Cff;" class="fa fa-folder"></i> ' + splitFiles[i] + '</div>';
                        } else {
                            splitFilesOriginal = splitFiles[i];
                            splitFiles[i] = splitFiles[i].split("\\");
                            splitFiles[i] = splitFiles[i][splitFiles[i].length - 1];
                            toAddtxt = "openFile('" + splitFilesOriginal + "');";
                            toAddtxt = toAddtxt.replaceAll("\\", "\\\\");
                            let icon = chooseIcon(splitFiles[i]);
                            if (iswholedevicesearch) {
                                splitFiles[i] += "<small style='color: grey;'>&nbsp; " + splitFilesOriginal + "</small>";
                            }
                            toAdd = '<div class="file" onclick="' + toAddtxt + '"><input type="checkbox" onclick="event.stopPropagation();">' + icon + ' ' + splitFiles[i] + '</div>';
                        }                    
                        document.getElementById("filelist").innerHTML += toAdd;
                    }
                    iswholedevicesearch = false;
                }

                function openFolder(currentFolder) {
                    document.getElementById("filelist").innerHTML = "";
                    // Get the files in current folder
                    URL = "http://localhost:1104/retrievefolder?folder=" + currentFolder;
                    $.get(URL,{ "_": $.now() }, function(response) {
                        if (response == "ERROR") {
                            document.getElementById("filelist").innerHTML = "There was an error. This directory may have been deleted, or FastBrowse doesn't have permissions to access the folder.";
                        } else if (response == "") {
                            document.getElementById("filelist").innerHTML = "There aren't any files in this folder.";
                        } else {
                            currentDirectory = currentFolder;
                            document.getElementById("mandirbox").value = currentDirectory; 
                            displayFiles(response);
                        }
                    });
                }

                function refreshFiles() {
                    openFolder(currentDirectory);
                }

                function getSelectedFiles() {
                    // Select all checkboxes
                    const checkboxes = document.querySelectorAll('input[type="checkbox"]');
                    // Filter out the selected checkboxes
                    const selectedCheckboxes = Array.from(checkboxes).filter(checkbox => checkbox.checked);
                    // For each checkbox, get the file box that it's in, and get it's whole file path
                    let selectedPaths = [];
                    for (let i = 0; i < selectedCheckboxes.length; i++) {
                        selectedPaths.push(selectedCheckboxes[i].parentNode.getAttribute("onclick").split("'")[1].split("'")[0]);
                    }
                    return selectedPaths;
                }

                async function httpRename(from, newFileName) {
                    let URL = ("http://localhost:1104/renamefile?from=" + from + "&to=" + currentDirectory + newFileName).replaceAll(" ", "%20");
                    $.get(URL, { "_": $.now() }, function (response) {
                        if (response == "ERROR") {
                            alert("There was an error. A file with this name may already exist.");
                        }
                    });
                }
                
                async function renameSelected() {
                    // Make sure only one item is selected
                    let selectedFiles = getSelectedFiles();
                    if (selectedFiles.length != 1) {
                        alert("You can only rename one file at a time.");
                    } else {
                        // Call the async function to rename the file, then refresh
                        await httpRename(selectedFiles[0], prompt("Choose a new file name: "));
                        refreshFiles();
                    }
                }

                function copySelected() {
                    clipboardFiles = getSelectedFiles();
                }

                function cutSelected() {
                    copySelected();
                    clipboardIsCut = true;
                    alert("Copied to clipboard. The original file(s) will be deleted once you paste them in a new directory.");
                }

                async function doPasteSelected() {
                    // For each file, make a copy to the right location
                    for (let f = 0; f < clipboardFiles.length; f++) {
                        URL = ("http://localhost:1104/pastefile?from=" + clipboardFiles[f] + "&to=" + currentDirectory).replaceAll(" ", "%20");
                        $.get(URL, { "_": $.now() }, function(response) {});
                    }
                }

                async function pasteSelected() {
                    await doPasteSelected();
                    alert("Pasted " + clipboardFiles.length + " files to " + currentDirectory);
                    if (clipboardIsCut) {
                        // Delete the original files
                        for (let f = 0; f < clipboardFiles.length; f++) {
                            URL = ("http://localhost:1104/deletefile?fname=" + clipboardFiles[f]).replaceAll(" ", "%20");
                            $.get(URL, { "_": $.now() }, function(response) {
                                console.log("Deleted " + clipboardFiles[f]);
                            });
                        }
                        clipboardIsCut = false;
                    }
                    refreshFiles();
                }

                async function doActualDeletion() {
                    let selectedFiles = getSelectedFiles();
                    for (let f = 0; f < selectedFiles.length; f++) {
                        URL = ("http://localhost:1104/deletefile?fname=" + selectedFiles[f]).replaceAll(" ", "%20");
                        $.get(URL, { "_": $.now() }, function(response) {
                            console.log("Deleted " + selectedFiles[f]);
                        });
                    }
                }

                async function deleteSelected() {
                    if (confirm("Are you sure you want to delete " + getSelectedFiles().length + " file(s)?")) {
                        await doActualDeletion();
                        refreshFiles();
                        console.log("Deleted " + selectedFiles.length + " files.");
                    }
                }

                function createFile() {
                    // Ask for a file name
                    let filename = prompt("Enter a filename: ");
                    // Create a path
                    let full = currentDirectory + filename;
                    console.log("Creating file: " + full);
                    // Make a HTTP request
                    let URL = ("http://localhost:1104/createfile?fname=" + full).replaceAll(" ", "%20");
                    $.get(URL, { "_": $.now() }, function(response) {
                        console.log("New file created. Refreshing...");
                        refreshFiles();
                    });
                }

                function createFolder() {
                    // Ask for a folder name
                    let foldername = prompt("Enter a folder name: ");
                    // Create a path
                    let full = currentDirectory + foldername;
                    console.log("Creating folder: " + full);
                    let URL = ("http://localhost:1104/createfolder?dname=" + full).replaceAll(" ", "%20");
                    $.get(URL, { "_": $.now() }, function(response) {
                        console.log("New folder created. Refreshing...");
                        refreshFiles();
                    });
                }

                function openmandir() {
                    let newFolder = (document.getElementById("mandirbox").value).replaceAll(" ", "%20");
                    let URL = "http://localhost:1104/retrievefolder?folder=" + newFolder;
                    if (URL[URL.length - 1] != "\\") {
                        URL += "\\";
                    }
                    console.log("Making HTTP request to " + URL);
                    $.get(URL,{ "_": $.now() }, function(response) {
                        if (response != "") {
                            displayFiles(response);
                        } else {
                            console.log("It's empty");
                        }
                    });
                }

                function searchCD() {
                    k = document.getElementById("searchbox").value; // query
                    let URL = ("http://localhost:1104/search?k=" + k + "&d=" + currentDirectory).replaceAll(" ", "%20");
                    $.get(URL,{ "_": $.now() }, function(response) {
                        if (response != "") {
                            document.getElementById("filelist").innerHTML = "";
                            displayFiles(response, issearch = true);
                        } else {
                            document.getElementById("filelist").innerHTML = "No search results were found. Try adjusting your search.";
                        }
                    });
                }

                function searchDevice() {
                    k = document.getElementById("searchbox").value;
                    let URL = ("http://localhost:1104/searchdevice?k=" + k).replaceAll(" ", "%20");
                    console.log(URL);
                    document.getElementById("filelist").innerHTML = "Searching your device...";
                    $.get(URL,{ "_": $.now() }, function(response) {
                        document.getElementById("filelist").innerHTML = "";
                        iswholedevicesearch = true;
                        displayFiles(response);
                    });
                }

                function terminalOpenCD() {
                    let URL = "http://localhost:1104/terminalopencd?cd=" + currentDirectory.replaceAll("\\", "\\\\");
                    console.log("Making request to " + URL);
                    $.get(URL,{ "_": $.now() }, function(response) {
                        console.log("Opened CD in terminal.");
                    });
                }

                function submitSearch() {
                    // Check if it should search the current folder or the whole device
                    if (document.getElementById("searchScope").value == "This folder") {
                        searchCD();
                    } else {
                        searchDevice();
                    }
                }

                function openPastUser(ext) {
                    let URL = "http://localhost:1104/getuser";
                    let currentUser = "";
                    $.get(URL,{ "_": $.now() }, function(response) {
                        responseN = response.slice(0,-1);
                        currentUser = "C:\\Users\\" + responseN + "\\" + ext + "\\";
                        console.log("Replaced: " + currentUser);
                        openFolder(currentUser);
                    });
                }

                function goForward() {
                    if (lastBackFrom.length > 0) {
                        // Go to the last "backwarded" directory
                        openFolder(lastBackFrom[lastBackFrom.length - 1]);
                        // Remove from list
                        lastBackFrom.pop();
                    }
                }

                function goBack() {
                    // Check it's not in the "root" directory
                    // First get the username
                    let URL = "http://localhost:1104/getuser";
                    $.get(URL,{ "_": $.now() }, function(response) {
                        let responseN = response.slice(0,-1);
                        let currentUser = "C:\\Users\\" + responseN + "\\";
                        // Check that the current directory isn't currentUser
                        if (currentUser != currentDirectory) {
                            // Add it to the previous back list (so that user can go forward again)
                            lastBackFrom.push(currentDirectory);
                            // Go to the new directory
                            let newDir = currentDirectory.split("\\")
                            for (let n = 0; n < 2; n++) {
                                newDir.pop();
                            }
                            openFolder(newDir.join("\\") + "\\");
                        }
                    });
                }

                function openFile(filePath) {
                    console.log("Opening file: " + filePath);
                    let URL = "http://localhost:1104/openfile?filename=" + filePath;
                    $.get(URL,{ "_": $.now() }, function(response) {
                        console.log("Successfully opened.");
                    });
                }
                // Get current user
                let URL = "http://localhost:1104/getuser";
                let currentUser = "";
                $.get(URL,{ "_": $.now() }, function(response) {
                    currentUser = response;
                    currentUser = currentUser.slice(0, -1);
                    let currentFolder = "C:\\Users\\" + currentUser + "\\";
                    currentDirectory = currentFolder;
                    // Render the file list
                    openFolder(currentFolder);
                });