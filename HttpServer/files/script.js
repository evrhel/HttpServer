let url = "http://localhost/post/json";

let xhr = new XMLHttpRequest();
xhr.open("POST", url, true);


xhr.setRequestHeader("Accept", "application/json");
xhr.setRequestHeader("Content-Type", "application/json");

xhr.onreadystatechange = function() {
	if (xhr.readyState === 4) {
		console.log(xhr.responseText);
	}
};

let data = JSON.stringify({
	value: "value"
});

xhr.send(data);