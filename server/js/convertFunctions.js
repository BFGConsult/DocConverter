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
    document.getElementById('suffix').value=targetformat;
    var form = document.getElementById('form');
    form.action = window.location + targetformat;
    console.log(form.action);
    var submit = document.getElementById('submit');
    submit.disabled = false;
}

var cursuffix = '';


function setTargets() {
    var suffix = document.getElementById('fileinput').value.split('.').pop();
    var initialsuffix=document.getElementById('suffix').value;
    if (suffix == cursuffix)
	return;
    cursuffix = suffix;
    var targets = document.getElementById('targets');
    var submit = document.getElementById('submit');
    submit.disabled = true;
    
    removeOptions(targets);
    targets.disabled = true;
    if (suffix) {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	    if (this.readyState == 4 && this.status == 200) {
		var suffixes = JSON.parse(xhttp.responseText);
		var suffixKeys = Object.keys(suffixes);
		if (suffixKeys.length > 0) {
		    //suffixKeys.sort();
		    for (key of suffixKeys) {
			var option = document.createElement("option");
			var suffix=suffixes[key];
			option.text = suffix['pretty'];
			option.value = suffix['extension'];
			targets.add(option); 
		    }
		    targets.disabled = false;
		    targets.value=initialsuffix;
		    setAction();
		}
	    }
	};
	xhttp.open("GET", window.location + "list/targets/" + suffix + "/", true);
	xhttp.send();
    }
}
