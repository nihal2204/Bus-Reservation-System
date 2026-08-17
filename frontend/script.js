const API = "http://localhost:18080";


// View buses
function showBuses() {

    fetch(API + "/buses")
        .then(response => response.text())
        .then(data => {

            document.getElementById("result")
                .textContent = data;
        })
        .catch(error => {

            document.getElementById("result")
                .textContent =
                "Unable to connect to C++ backend";
        });
}


// Book ticket
function bookTicket() {

    let passenger =
        document.getElementById("passenger").value;

    let bus =
        document.getElementById("bus").value;

    let seat =
        document.getElementById("seat").value;

    let data =
        "passenger=" + encodeURIComponent(passenger) +
        "&bus=" + bus +
        "&seat=" + seat;

    fetch(API + "/book", {

        method: "POST",

        headers: {
            "Content-Type":
                "application/x-www-form-urlencoded"
        },

        body: data

    })
    .then(response => response.text())
    .then(result => {

        document.getElementById("result")
            .textContent = result;
    });
}


// Cancel ticket
function cancelTicket() {

    let id =
        document.getElementById("bookingId").value;

    let data = "id=" + id;

    fetch(API + "/cancel", {

        method: "POST",

        headers: {
            "Content-Type":
                "application/x-www-form-urlencoded"
        },

        body: data

    })
    .then(response => response.text())
    .then(result => {

        document.getElementById("result")
            .textContent = result;
    });
}