// Funktion zum Belegen eines Parkplatzes
function park(etage, id) {
    fetch('/park', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ etage: etage, id: id }),
    })
    .then(response => response.json())
    .then(data => {
        console.log(data);
        location.reload(); // Seite neu laden nach erfolgreicher Aktion
    })
    .catch(error => console.error('Error:', error));
}

// Funktion zum Freigeben eines Parkplatzes
function leave(etage, id) {
    fetch('/leave', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({ etage: etage, id: id}),
    })
    .then(response => response.json())
    .then(data => {
        console.log(data);
        location.reload(); // Seite neu laden nach erfolgreicher Aktion
    })
    .catch(error => console.error('Error:', error));
}

// Event-Listener für das Hinzufügen eines neuen Parkplatzes
document.getElementById('addParkplatzForm').addEventListener('submit', function(event) {
    event.preventDefault();  // Verhindert das Neuladen der Seite

    // Werte aus dem Formular
    const etage = document.getElementById('etage').value;

    // POST-Anfrage an die API zum Hinzufügen eines neuen Parkplatzes
    fetch('/api/add_parkplatz', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: JSON.stringify({
            etage: etage
        }),
    })
    .then(response => response.json())
    .then(data => {
        alert(`Neuer Parkplatz mit ID ${data.id} hinzugefügt!`);  // Erfolgsmeldung anzeigen
        location.reload();  // Seite neu laden, um den neuen Parkplatz anzuzeigen
    })
    .catch(error => console.error('Error:', error));
});

