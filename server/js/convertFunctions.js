function removeOptions(selectbox)
{
    var i;
    for(i = selectbox.options.length - 1 ; i >= 0 ; i--)
    {
        selectbox.remove(i);
    }
}

function setAction() {
    var targetformat = document.getElementById('targets').value;
    var form = document.getElementById('form');
    form.action = window.location + targetformat;
    console.log(form.action);
    var submit = document.getElementById('submit');
    submit.disabled = false;
}

function setTargets() {
    var suffix = document.getElementById('fileinput').value.split('.').pop();
    var targets = document.getElementById('targets');
    var submit = document.getElementById('submit');
    submit.disabled = true;
    
    removeOptions(targets);
    targets.disabled = true;
    if (suffix) {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	    if (this.readyState == 4 && this.status == 200) {
		var suffixes = Object.keys(JSON.parse(xhttp.responseText));
		if (suffixes.length > 0) {
		    suffixes.sort();
		    for (suffix of suffixes) {
			var option = document.createElement("option");
			option.text = suffix;
			targets.add(option); 
		    }
		    targets.disabled = false;
		    setAction();
		}
	    }
	};
	xhttp.open("GET", window.location + "list/targets/" + suffix + "/", true);
	xhttp.send();
    }
}
