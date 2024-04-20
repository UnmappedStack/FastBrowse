        let showingExtra = false;
        function toggleShowingExtra() {
            if (showingExtra == false) {
                showingExtra = true;
                document.getElementById("newtypes").style = "opacity: 100%;";
            } else {
                showingExtra = false;
                document.getElementById("newtypes").style = "opacity: 0%;";
            }
        }
        function opencredits() {
            document.getElementById("creditsfloat").style = "display: block;";
        }
        function closecredits() {
            document.getElementById("creditsfloat").style = "display: none;";
        }