import asyncio
from json import dumps
from time import sleep

from aiocoap import *


async def send_boolean(key, value):
    context = await Context.create_client_context()
    payload = dumps({key: value}).encode("utf-8")

    request = Message(code=PUT, payload=payload,
                      uri="coap://192.168.0.100/boolean")
    response = await context.request(request).response

    print(f"Sent: {key}={value}")
    print(f"Response: {response.code} - {response.payload.decode('utf-8')}")

if __name__ == "__main__":
    asyncio.run(send_boolean("p1", True))
    asyncio.run(send_boolean("p2", False))
    asyncio.run(send_boolean("p3", True))
    asyncio.run(send_boolean("p4", True))
