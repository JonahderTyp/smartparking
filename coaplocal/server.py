import asyncio
import json
from datetime import datetime

from aiocoap import *
from aiocoap.resource import *


class BooleanResource(Resource):
    def __init__(self):
        super().__init__()
        self.values = {"p1": False, "p2": False,
                       "p3": True, "p4": True}

    async def render_put(self, request):
        print(datetime.now())
        print(f"Received: {request.payload}")
        data: dict = json.loads(request.payload)
        for key, value in self.values.items():
            self.values[key] = bool(data.get(key, value))
            print(f"Updated: {key} = {self.values[key]}")
        return Message(code=CHANGED, payload=b"Value updated.")
        payload = request.payload.decode("utf-8")
        key, value = payload.split("=")
        self.values[key] = value.lower() == "true"
        print(f"Updated: {key} = {self.values[key]}")
        return Message(code=CHANGED, payload=b"Value updated.")

    async def render_get(self, request):
        response = json.dumps(self.values)
        # response = ", ".join([f"{k}={v}" for k, v in self.values.items()])
        print(f"{datetime.now()} Sent: {response}")
        return Message(payload=response.encode("utf-8"))


async def main():
    root = Site()
    root.add_resource(['boolean'], BooleanResource())

    await Context.create_server_context(root)

    print("CoAP server running...")
    await asyncio.get_running_loop().create_future()  # Run forever

if __name__ == "__main__":

    v = {"p1": False, "p2": False,
         "p3": True, "p4": True}
    print(json.dumps(v))

    asyncio.run(main())
