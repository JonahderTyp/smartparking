from flask import Flask, render_template, jsonify, request
from flask_sqlalchemy import SQLAlchemy

app = Flask(__name__)

# SQLite-Datenbank
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///identifier.sqlite'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db = SQLAlchemy(app)

# Parkplatz-Datenmodell
class Parkplatz(db.Model):
    id = db.Column(db.Integer, primary_key=True, autoincrement=True)  # ID wird automatisch inkrementiert
    etage = db.Column(db.Integer, nullable=False)
    status = db.Column(db.Boolean, default=1)

    def __repr__(self):
        return f'<Parkplatz {self.id} - {self.etage} - {"Belegt" if self.status else "Frei"}>'

# Erstelle die Tabelle, falls sie noch nicht existiert
with app.app_context():
    db.create_all()

### Web-basierte Schnittstellen ###

@app.route('/')
def index():
    # Lade die Parkplatzinformationen aus der SQLite-Datenbank
    parkplaetze = Parkplatz.query.all()
    return render_template('index.html', parkplaetze=parkplaetze)

# Parkplatz belegen
@app.route('/park', methods=['POST'])
def park():
    data = request.json
    parkplatz_etage = data.get('etage')
    parkplatz_id = data.get('id')

    # Ändere den Status des Parkplatzes auf belegt (1)
    parkplatz = Parkplatz.query.filter_by(etage=parkplatz_etage, id=parkplatz_id).first()
    if parkplatz:
        parkplatz.status = 0  # Wenn der Parkplatz gefunden wird
    else:
        # Neuen Parkplatz hinzufügen
        parkplatz = Parkplatz(etage=parkplatz_etage, id=parkplatz_id, status=0)
        db.session.add(parkplatz)

    db.session.commit()
    return jsonify({'message': 'Parkplatz belegt'}), 200

# Parkplatz freigeben
@app.route('/leave', methods=['POST'])
def leave():
    data = request.json
    parkplatz_etage = data.get('etage')
    parkplatz_id = data.get('id')

    # Ändere den Status des Parkplatzes auf frei (1)
    parkplatz = Parkplatz.query.filter_by(etage=parkplatz_etage, id=parkplatz_id).first()
    if parkplatz:  # Wenn der Parkplatz existiert
        parkplatz.status = 1  # Parkplatz freigeben (1 = frei)
        db.session.commit()
        return jsonify({'message': 'Parkplatz freigegeben'}), 200
    else:
        return jsonify({'error': 'Parkplatz nicht gefunden'}), 404

### API-Schnittstellen ###

# Alle Parkplätze abfragen
@app.route('/api/parkplaetze', methods=['GET'])
def get_parkplaetze():
    parkplaetze = Parkplatz.query.all()
    result = [
        {
            'id': platz.id,
            'etage': platz.etage,
            'status': platz.status
        } for platz in parkplaetze
    ]
    return jsonify(result), 200

# Parkplatz belegen (API)
@app.route('/api/park', methods=['POST'])
def api_park():
    data = request.json
    parkplatz_etage = data.get('etage')
    parkplatz_id = data.get('id')

    # Ändere den Status des Parkplatzes auf belegt (0)
    parkplatz = Parkplatz.query.filter_by(etage=parkplatz_etage, id=parkplatz_id).first()
    if parkplatz:  # Überprüfe, ob ein Parkplatz gefunden wurde
        parkplatz.status = 0
        db.session.commit()
        return jsonify({'message': 'Parkplatz belegt'}), 200
    else:
        return jsonify({'error': 'Parkplatz nicht gefunden'}), 404

# Parkplatz freigeben (API)
@app.route('/api/leave', methods=['POST'])
def api_leave():
    data = request.json
    parkplatz_etage = data.get('etage')
    parkplatz_id = data.get('id')


    # Ändere den Status des Parkplatzes auf frei (1)
    parkplatz = Parkplatz.query.filter_by(etage=parkplatz_etage, id=parkplatz_id).first()
    if parkplatz:
        parkplatz.status = 1
        db.session.commit()
        return jsonify({'message': 'Parkplatz freigegeben'}), 200
    else:
        return jsonify({'error': 'Parkplatz nicht gefunden'}), 404

# Parkplatzstatus aktualisieren (API)
@app.route('/api/update_parkplatz', methods=['PUT'])
def update_parkplatz():
    data = request.json
    parkplatz_etage = data['etage']
    prakplatz_status = data['status']  # 1 = frei, 0 = belegt
    parkplatz_id = data['id']

    # Überprüfe, ob der Parkplatz bereits existiert, wenn ja, aktualisiere ihn
    parkplatz = Parkplatz.query.filter_by(etage=parkplatz_etage, id=parkplatz_id).first()
    if not parkplatz:
        parkplatz.status = prakplatz_status
    else:
        # Wenn der Parkplatz nicht existiert, erstelle einen neuen
        parkplatz = Parkplatz(etage=parkplatz_etage, status=prakplatz_status)
        db.session.add(parkplatz)

    db.session.commit()
    return jsonify({'message': 'Parkplatzstatus aktualisiert'}), 200


# Route zum Hinzufügen eines neuen Parkplatzes
# Route zum Hinzufügen eines neuen Parkplatzes
@app.route('/api/add_parkplatz', methods=['POST'])
def add_parkplatz():
    data = request.json  # JSON-Daten empfangen
    parkplatz_etage = data.get('etage')

    # Neuen Parkplatz erstellen (ID wird automatisch inkrementiert)
    neuer_parkplatz = Parkplatz(etage=parkplatz_etage, status=1)  # Standardstatus: frei
    db.session.add(neuer_parkplatz)
    db.session.commit()

    return jsonify({'message': 'Neuer Parkplatz hinzugefügt', 'id': neuer_parkplatz.id}), 201



if __name__ == '__main__':
    app.run(debug=True)
