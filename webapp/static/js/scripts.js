// Funktion zum Abfragen des Parkplatzstatus
function fetchStatus() {
    fetch('/api/status')  // API, um den Status aller Parkplätze zu erhalten
        .then(response => response.json())
        .then(data => {
            Object.keys(data).forEach((platzId) => {
                const statusElement = document.getElementById(`status-${platzId}`);
                const parkButton = document.querySelector(`#platz-${platzId} .park-button`);
                const leaveButton = document.querySelector(`#platz-${platzId} .leave-button`);

                if (data[platzId] === "besetzt") {
                    statusElement.textContent = 'Besetzt';
                    statusElement.classList.add('besetzt');
                    statusElement.classList.remove('frei');

                    // "Parken"-Button deaktivieren, "Verlassen"-Button aktivieren
                    parkButton.disabled = true;
                    parkButton.classList.remove('aktiv');
                    leaveButton.disabled = false;
                    leaveButton.classList.add('aktiv');
                } else {
                    statusElement.textContent = 'Frei';
                    statusElement.classList.remove('besetzt');
                    statusElement.classList.add('frei');

                    // "Parken"-Button aktivieren, "Verlassen"-Button deaktivieren
                    parkButton.disabled = false;
                    parkButton.classList.add('aktiv');
                    leaveButton.disabled = true;
                    leaveButton.classList.remove('aktiv');
                }
            });
        });
}

// API-Aufruf zum Parken
function park(platzId) {
    fetch(`/api/park/${platzId}`, { method: 'POST' })
        .then(response => response.json())
        .then(data => {
            console.log(data.message);
            fetchStatus();  // Den Status nach dem Parken sofort aktualisieren
        });
}

// API-Aufruf zum Verlassen
function leave(platzId) {
    fetch(`/api/leave/${platzId}`, { method: 'POST' })
        .then(response => response.json())
        .then(data => {
            console.log(data.message);
            fetchStatus();  // Den Status nach dem Verlassen sofort aktualisieren
        });
}

// Status alle 1 Sekunde aktualisieren
setInterval(fetchStatus, 1000);
