from flask import Flask, render_template, jsonify, request

app = Flask(__name__)

# Parkplätze (Hardcoded)
parkplaetze = {
    1: "frei",
    2: "frei",
    3: "frei",
    4: "frei"
}

@app.route('/')
def index():
    return render_template('index.html', parkplaetze=parkplaetze)

# API-Schnittstelle für das Parken
@app.route('/api/park/<int:platz_id>', methods=['POST'])
def park(platz_id):
    if platz_id in parkplaetze:
        parkplaetze[platz_id] = 'besetzt'
        return jsonify({"status": "success", "message": f"Parkplatz {platz_id} besetzt"}), 200
    return jsonify({"status": "error", "message": "Ungültiger Parkplatz"}), 400

# API-Schnittstelle für das Verlassen
@app.route('/api/leave/<int:platz_id>', methods=['POST'])
def leave(platz_id):
    if platz_id in parkplaetze:
        parkplaetze[platz_id] = 'frei'
        return jsonify({"status": "success", "message": f"Parkplatz {platz_id} frei"}), 200
    return jsonify({"status": "error", "message": "Ungültiger Parkplatz"}), 400

# API zum Abrufen des aktuellen Status aller Parkplätze
@app.route('/api/status', methods=['GET'])
def status():
    return jsonify(parkplaetze)

if __name__ == '__main__':
    app.run()
