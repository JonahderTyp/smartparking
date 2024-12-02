import asyncio
import json
from typing import Dict

from aiocoap import *
from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

app.config['DEBUG'] = True

data: Dict[str, bool] = {"p1": False, "p2": False,
                         "p3": False, "p4": False}


async def get_coap_data():
    global data
    context = await Context.create_client_context()

    request = Message(code=GET, uri="coap://localhost/boolean")
    response = await context.request(request).response

    print("Received:")
    print(response.payload.decode("utf-8"))

    data = json.loads(response.payload.decode("utf-8"))

    print(f"Data: {data}")


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


@app.route('/api/data')
def get_sensor_data():
    global data
    asyncio.run(get_coap_data())

    return jsonify({(int(key.replace("p", ""))): val for key, val in data.items()})


@app.route('/api/status', methods=['GET'])
def status():
    return jsonify(parkplaetze)


if __name__ == '__main__':
    app.run()
